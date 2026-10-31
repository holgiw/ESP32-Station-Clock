#pragma once

    // MQTT-Meldung der Uhr (z. B. fuer openHAB): Die Uhr meldet ihren Status an einen MQTT-Broker - einmal nach dem
    // Start, einmal um Mitternacht und wenn sich ihre IP-Adresse aendert. Ein schlanker Sende-Client (MQTT 3.1.1, QoS 0,
    // retained) ohne Bibliothek: verbinden, melden, trennen. Der Broker haelt die Werte (retained). Sie stehen unter
    // <Basis-Topic>/<Hostname>/<Name>: info (JSON mit host, ip, chip, display, version - daraus baut openHAB die
    // Uhrenliste), ip, host, chip, display, version, time_ok, heap_kb, rssi, uptime_s. Ob die Uhr gerade laeuft, zeigt das Ping-Geraet in openHAB, nicht diese Werte. Standardmaessig
    // aus; Einstellungen auf der Seite /mqtt. Das Passwort steht nicht in der Sicherung. Schlaegt eine Meldung fehl,
    // folgt der naechste Versuch nach 2, 4, 8 und dann alle 15 Minuten.

    // MQTT report of the clock (e.g. for openHAB): the clock reports its status to an MQTT broker - once after the
    // start, once at midnight and when its IP address changes. A slim send-only client (MQTT 3.1.1, QoS 0, retained)
    // without a library: connect, report, disconnect. The broker keeps the values (retained). They are under
    // <base topic>/<host name>/<name>: info (JSON with host, ip, chip, display, version - openHAB builds the clock
    // list from it), ip, host, chip, display, version, time_ok, heap_kb, rssi, uptime_s. Whether the clock is running right now is shown by the ping device in openHAB, not by
    // these values. Off by default; settings on the page /mqtt. The password is not part of the backup. If a report
    // fails, the next attempt follows after 2, 4, 8 and then every 15 minutes.

#define MQTT_RETRY_MS (2 * WAIT_1m) // erster Wiederholungsabstand, verdoppelt sich bis 15 Minuten / first retry interval, doubles up to 15 minutes

    bool mqttEnabled = false;
    String mqttHost = "";
    String mqttUser = "";
    String mqttPass = "";
    String mqttPrefix = "uhr4";
    uint16_t mqttPort = 1883;
    String mqttLastResult = "";        // "OK" oder Fehlertext / "OK" or error text
    unsigned long mqttLastAttempt = 0; // millis() des letzten Versuchs, 0 = noch keiner / millis() of the last attempt, 0 = none yet
    unsigned long mqttNextAt = 0;      // millis() des naechsten Versuchs, 0 = noch nicht geplant / millis() of the next attempt, 0 = not planned
    uint8_t mqttFails = 0;
    bool mqttPending = true;           // eine Meldung steht an (Start, Mitternacht, neue IP) / a report is due (start, midnight, new IP)
    int mqttLastDay = -1;              // Tag des Jahres der letzten Meldung / day of the year of the last report
    uint32_t mqttLastIp = 0;           // IP-Adresse der letzten Meldung / IP address of the last report

    void loadMqttSettings() {
        mqttEnabled = preferences.getBool(PK_MQTT_ENABLED, false);
        mqttHost = preferences.getString(PK_MQTT_HOST, "");
        mqttPort = preferences.getUShort(PK_MQTT_PORT, 1883);
        mqttUser = preferences.getString(PK_MQTT_USER, "");
        mqttPass = preferences.getString(PK_MQTT_PASS, "");
        mqttPrefix = preferences.getString(PK_MQTT_PREFIX, "uhr4");
        if (mqttPrefix.isEmpty()) mqttPrefix = "uhr4";
        mqttNextAt = 0;
        mqttFails = 0;
        mqttPending = true; // nach Aenderung der Einstellungen gleich neu melden / report again right after a settings change
    }

    // Ein MQTT-Paket in einem festen Puffer aufbauen (Zeichenketten mit 16-Bit-Laenge davor)
    // Build an MQTT packet in a fixed buffer (strings with a 16-bit length in front)

    struct MqttPacket {
        uint8_t data[360];
        size_t len = 0;
        bool overflow = false;
        void put(uint8_t b) { if (len < sizeof(data)) data[len++] = b; else overflow = true; }
        void putString(const String& s) {
            put((uint8_t)(s.length() >> 8));
            put((uint8_t)(s.length() & 0xFF));
            for (size_t i = 0; i < s.length(); i++) put((uint8_t)s[i]);
        }
    };

    // Kopf (Typ + "verbleibende Laenge" als Varint) und Inhalt senden
    // Send the header (type + "remaining length" as varint) and the content

    bool mqttSendPacket(WiFiClient& client, uint8_t type, const MqttPacket& body) {
        if (body.overflow) return false;
        uint8_t header[5];
        size_t h = 0;
        header[h++] = type;
        size_t rest = body.len;
        do {
            uint8_t b = rest % 128;
            rest /= 128;
            if (rest) b |= 0x80;
            header[h++] = b;
        } while (rest && h < sizeof(header));
        return client.write(header, h) == h && client.write(body.data, body.len) == body.len;
    }

    bool mqttPublish(WiFiClient& client, const String& topic, const String& payload) {
        MqttPacket body;
        body.putString(topic);
        for (size_t i = 0; i < payload.length(); i++) body.put((uint8_t)payload[i]);
        return mqttSendPacket(client, 0x31, body); // PUBLISH, QoS 0, retained / PUBLISH, QoS 0, retained
    }

    // Einmal verbinden, den Status melden und trennen. true = alles gesendet, sonst steht der Grund in error.
    // Connect once, report the status and disconnect. true = everything sent, otherwise the reason is in error.

    bool mqttPublishStatus(String& error) {
        if (mqttHost.isEmpty()) { error = "no broker address"; return false; }
        if (WiFi.status() != WL_CONNECTED) { error = "no WiFi"; return false; }
        IPAddress broker;
        if (!broker.fromString(mqttHost) && !WiFi.hostByName(mqttHost.c_str(), broker)) { error = "broker name not found"; return false; }

        WiFiClient client;
        client.setTimeout(1000); // Millisekunden (Stream::setTimeout) / milliseconds (Stream::setTimeout)
        if (!client.connect(broker, mqttPort, 1000)) { error = "no connection to the broker"; return false; }
        client.setNoDelay(true);

        MqttPacket connect;
        connect.putString("MQTT");
        connect.put(4);                                         // Protokollstufe 3.1.1 / protocol level 3.1.1
        connect.put(0x02 | (mqttUser.length() ? 0x80 : 0) | (mqttUser.length() && mqttPass.length() ? 0x40 : 0));
        connect.put(0); connect.put(60);                        // Keep-Alive 60 s / keep alive 60 s
        connect.putString(String("uhr4-") + hostname);
        if (mqttUser.length()) connect.putString(mqttUser);
        if (mqttUser.length() && mqttPass.length()) connect.putString(mqttPass);
        if (!mqttSendPacket(client, 0x10, connect)) { client.stop(); error = "send error"; return false; }

        uint8_t ack[4] = { 0 };
        if (client.readBytes(ack, 4) != 4 || ack[0] != 0x20) { client.stop(); error = "no answer from the broker"; return false; }
        if (ack[3] != 0) { client.stop(); error = "broker refused the login (code " + String(ack[3]) + ")"; return false; }

        String base = mqttPrefix + "/" + hostname + "/";
        struct tm now;
        bool haveTime = getLocalTime(&now, 0);

        bool ok = true;
        ok = ok && mqttPublish(client, base + "info", String("{\"host\":\"") + hostname + "\",\"ip\":\"" + WiFi.localIP().toString() + "\",\"chip\":\"" + CHIP_SHORT_NAME +
                                                          "\",\"display\":\"" + displayChoiceName(displayType, useBacklight) + "\",\"version\":\"" + version + "\"}");
        ok = ok && mqttPublish(client, base + "ip", WiFi.localIP().toString());
        ok = ok && mqttPublish(client, base + "host", String(hostname));
        ok = ok && mqttPublish(client, base + "chip", String(CHIP_SHORT_NAME));
        ok = ok && mqttPublish(client, base + "display", String(displayChoiceName(displayType, useBacklight)));
        ok = ok && mqttPublish(client, base + "version", String(version));
        ok = ok && mqttPublish(client, base + "time_ok", (haveTime && timeTrusted()) ? "ON" : "OFF");
        ok = ok && mqttPublish(client, base + "heap_kb", String(ESP.getFreeHeap() / 1024));
        ok = ok && mqttPublish(client, base + "rssi", String(WiFi.RSSI()));
        ok = ok && mqttPublish(client, base + "uptime_s", String(millis() / 1000));
        uint8_t bye[2] = { 0xE0, 0x00 };                        // DISCONNECT
        client.write(bye, 2);
        client.flush();
        client.stop();
        if (!ok) { error = "send error"; return false; }
        return true;
    }

    // Aus loop(): prueft einmal pro Sekunde, ob eine Meldung ansteht - nach dem Start (20 s Verzug), um Mitternacht
    // (Ortszeit, sobald die Zeit bestaetigt ist) oder bei geaenderter IP-Adresse. Nicht waehrend der Zeigerfahrt und
    // nicht waehrend eines Firmware-Updates.

    // From loop(): checks once a second whether a report is due - after the start (20 s delay), at midnight (local
    // time, once the time is confirmed) or when the IP address changed. Not during the hand move and not during a
    // firmware update.

    void mqttTick() {
        if (!mqttEnabled || mqttHost.isEmpty()) return;
        if (otaInProgress || WiFi.status() != WL_CONNECTED || WiFi.getMode() == WIFI_AP || !backgroundWorkAllowed()) return;
        unsigned long now = millis();
        static unsigned long lastCheck = 0;
        if (now - lastCheck < WAIT_1s) return;
        lastCheck = now;

        if (mqttNextAt == 0) { mqttNextAt = now + 20 * WAIT_1s; if (mqttNextAt == 0) mqttNextAt = 1; }

        if (!mqttPending) {
            uint32_t ipNow = (uint32_t)WiFi.localIP();
            struct tm t;
            if (mqttLastIp != 0 && ipNow != mqttLastIp) mqttPending = true;
            else if (timeTrusted() && getLocalTime(&t, 0) && t.tm_hour == 0 && t.tm_yday != mqttLastDay) mqttPending = true;
            if (mqttPending) mqttNextAt = now;
            return;
        }
        if ((long)(now - mqttNextAt) < 0) return;

        String error;
        bool ok = mqttPublishStatus(error);
        mqttLastAttempt = now ? now : 1;
        mqttLastResult = ok ? String("OK") : error;
        if (ok) {
            if (mqttFails) DEBUG_PRINTLN("[MQTT] Broker reachable again");
            mqttFails = 0;
            mqttPending = false;
            mqttLastIp = (uint32_t)WiFi.localIP();
            struct tm t;
            mqttLastDay = getLocalTime(&t, 0) ? t.tm_yday : -1;
            mqttNextAt = 0;
        }
        else {
            if (mqttFails < 255) mqttFails++;
            unsigned long wait = MQTT_RETRY_MS << (mqttFails > 4 ? 4 : (mqttFails - 1)); // 2, 4, 8, 16 Min.
            if (wait > 15 * WAIT_1m) wait = 15 * WAIT_1m;
            mqttNextAt = now + wait;
            if (mqttNextAt == 0) mqttNextAt = 1;
            if (mqttFails == 1) DEBUG_PRINTLN("[MQTT] Report failed: " + error);
        }
    }

    // Kurztext fuer die Statusseite
    // Short text for the status page

    String mqttStatusText() {
        if (!mqttEnabled) return "off";
        if (mqttLastAttempt == 0) return "waiting for the first report";
        if (mqttPending && mqttLastResult != "OK") return mqttLastResult + " - retrying";
        String ago = String((millis() - mqttLastAttempt) / 1000) + " s ago";
        return mqttLastResult + " (" + ago + ")";
    }

    // Seite /mqtt: Einstellungen, Testknopf, Themenliste
    // Page /mqtt: settings, test button, topic list

    void setupMqttRoutes() {
        webserver.on("/mqtt", HTTP_GET, []() {

            // Kompakt in der Breite der Startseite (Karte, 500 px): Beschriftung links, Feld rechts
            // Compact in the width of the start page (card, 500 px): label on the left, field on the right

            String html = beginPage();
            html += generateFlashMessage();
            html += "<style>.mq .row{display:flex;align-items:center;gap:8px;margin:5px 0;}.mq .row>span{flex:0 0 130px;}"
                    ".mq input[type=text],.mq input[type=password],.mq input[type=number]{flex:1;width:auto;min-width:0;margin:0;padding:6px 8px;}"
                    ".mq input.port{flex:0 0 80px;}.mq button{width:auto;margin:8px 8px 0 0;padding:6px 16px;}.mq p{margin:6px 0;}</style>";
            html += "<h2>MQTT</h2><div class='card mq'>";
            html += "<p><small>" + translate("The clock reports its status to an MQTT broker, e.g. openHAB: once after the start, once at midnight and when its IP address changes. The broker keeps the values (retained) under the base topic and the host name of the clock") + ".</small></p>";
            html += "<form method='POST' action='/mqtt/save'>";
            html += "<div class='row'><label><input type='checkbox' name='enabled'" + String(mqttEnabled ? " checked" : "") + "> " + translate("Enable MQTT") + "</label></div>";
            html += "<div class='row'><span>" + translate("Broker address") + "</span><input type='text' name='host' value='" + escapeHtmlText(mqttHost) + "' maxlength='63' placeholder='192.168.0.70'>"
                    "<input class='port' type='number' name='port' min='1' max='65535' value='" + String(mqttPort) + "' title='Port'></div>";
            html += "<div class='row'><span>" + translate("User (optional)") + "</span><input type='text' name='user' value='" + escapeHtmlText(mqttUser) + "' maxlength='63' autocomplete='off'></div>";
            html += "<div class='row'><span>" + translate("Password") + "</span><input type='password' name='pass' maxlength='63' autocomplete='new-password' placeholder='" + translate("empty = unchanged") + "'>"
                    "<label title='" + translate("Delete password") + "'><input type='checkbox' name='clearPass'> " + translate("delete") + "</label></div>";
            html += "<div class='row'><span>" + translate("Base topic") + "</span><input type='text' name='prefix' value='" + escapeHtmlText(mqttPrefix) + "' maxlength='40'></div>";
            html += "<button type='submit'>" + translate("Save") + "</button>"
                    "<button type='submit' formaction='/mqtt/test'>" + translate("Send now") + "</button></form>";
            html += "<p><small>" + translate("Last result") + ": <b>" + escapeHtmlText(translate(mqttStatusText())) + "</b></small></p>";
            html += "<p><small>Topics: <code>" + escapeHtmlText(mqttPrefix) + "/" + escapeHtmlText(String(hostname)) + "/</code>"
                    "info, ip, host, chip, display, version, time_ok, heap_kb, rssi, uptime_s</small></p>";
            html += "<p><small>API: <code>curl -d \"broker=192.168.0.70&amp;port=1883\" http://" + WiFi.localIP().toString() + "/api/mqtt</code></small></p>";
            html += "</div></body></html>";
            webserver.send(200, "text/html", html);
            });

        webserver.on("/mqtt/save", HTTP_POST, []() {
            if (!isPrivateNetworkIp(webserver.client().remoteIP())) {
                webserver.send(403, "text/plain", "Only available from a private network");
                return;
            }
            String host = webserver.arg("host");
            host.trim();
            String prefix = webserver.arg("prefix");
            prefix.trim();
            String cleaned = "";
            for (size_t i = 0; i < prefix.length(); i++) {
                char c = prefix[i];
                if (isalnum((unsigned char)c) || c == '_' || c == '-' || c == '/') cleaned += c;
            }
            while (cleaned.startsWith("/")) cleaned.remove(0, 1);
            while (cleaned.endsWith("/")) cleaned.remove(cleaned.length() - 1);
            if (cleaned.isEmpty()) cleaned = "uhr4";
            long port = webserver.arg("port").toInt();
            if (port < 1 || port > 65535) port = 1883;

            preferences.putBool(PK_MQTT_ENABLED, webserver.hasArg("enabled"));
            preferences.putString(PK_MQTT_HOST, host);
            preferences.putUShort(PK_MQTT_PORT, (uint16_t)port);
            preferences.putString(PK_MQTT_USER, webserver.arg("user"));
            if (webserver.hasArg("clearPass")) preferences.putString(PK_MQTT_PASS, "");
            else if (webserver.arg("pass").length()) preferences.putString(PK_MQTT_PASS, webserver.arg("pass"));
            preferences.putString(PK_MQTT_PREFIX, cleaned);
            loadMqttSettings();
            redirectTo("/mqtt?msg=MQTT%20settings%20saved");
            });

        // /api/mqtt: nur Broker und Port lesen/setzen, ohne das ganze Formular (die anderen Einstellungen bleiben).
        // GET und POST (broker=, port=, Formular- oder Query-Parameter); "connected" = die letzte Meldung ging durch.
        // Beispiel: curl -d "broker=192.168.0.70" http://<ip>/api/mqtt

        // /api/mqtt: read/set only the broker and port, without the whole form (the other settings stay).
        // GET and POST (broker=, port=, form or query parameters); "connected" = the last report went through.
        // Example: curl -d "broker=192.168.0.70" http://<ip>/api/mqtt

        webserver.on("/api/mqtt", []() {
            webserver.sendHeader("Cache-Control", "no-store");
            auto answer = [](int code) {
                String host = mqttHost;
                host.replace("\\", "\\\\");
                host.replace("\"", "\\\"");
                webserver.send(code, "application/json", String("{\"broker\":\"") + host + "\",\"port\":" + String(mqttPort) +
                                                         ",\"connected\":" + (mqttEnabled && mqttLastResult == "OK" ? "true" : "false") + "}");
            };
            if (webserver.method() == HTTP_POST) {
                if (!isPrivateNetworkIp(webserver.client().remoteIP())) {
                    webserver.send(403, "application/json", "{\"error\":\"only from a private network\"}");
                    return;
                }
                String host = mqttHost;
                long port = mqttPort;
                if (webserver.hasArg("broker")) {
                    host = webserver.arg("broker");
                    host.trim();
                    bool valid = host.length() > 0 && host.length() <= 63;
                    for (size_t i = 0; valid && i < host.length(); i++) {
                        char c = host[i];
                        valid = isalnum((unsigned char)c) || c == '.' || c == '-' || c == '_' || c == ':';
                    }
                    if (!valid) {
                        webserver.send(400, "application/json", "{\"error\":\"broker invalid\"}");
                        return;
                    }
                }
                if (webserver.hasArg("port")) {
                    String portText = webserver.arg("port");
                    portText.trim();
                    port = portText.toInt();
                    bool digits = portText.length() > 0;
                    for (size_t i = 0; digits && i < portText.length(); i++) digits = isdigit((unsigned char)portText[i]);
                    if (!digits || port < 1 || port > 65535) {
                        webserver.send(400, "application/json", "{\"error\":\"port invalid\"}");
                        return;
                    }
                }
                if (webserver.hasArg("broker") || webserver.hasArg("port")) {
                    preferences.putString(PK_MQTT_HOST, host);
                    preferences.putUShort(PK_MQTT_PORT, (uint16_t)port);
                    loadMqttSettings();
                    DEBUG_PRINTLN("[MQTT] Broker set via API: " + host + ":" + String(port));
                }
            }
            answer(200);
            });

        webserver.on("/mqtt/test", HTTP_POST, []() {
            if (!isPrivateNetworkIp(webserver.client().remoteIP())) {
                webserver.send(403, "text/plain", "Only available from a private network");
                return;
            }
            String error;
            bool ok = mqttPublishStatus(error);
            mqttLastAttempt = millis() ? millis() : 1;
            mqttLastResult = ok ? String("OK") : error;
            if (ok) { mqttFails = 0; mqttPending = false; mqttLastIp = (uint32_t)WiFi.localIP(); mqttNextAt = 0; }
            redirectTo("/mqtt");
            });
    }
