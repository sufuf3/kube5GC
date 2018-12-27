ies = []
ies.append({ "iei" : "5B", "value" : "New EPS QoS", "type" : "EPS quality of service", "reference" : "9.9.4.3", "presence" : "O", "format" : "TLV", "length" : "3-15"})
ies.append({ "iei" : "36", "value" : "TFT", "type" : "Traffic flow template", "reference" : "9.9.4.16", "presence" : "O", "format" : "TLV", "length" : "3-257"})
ies.append({ "iei" : "30", "value" : "New QoS", "type" : "Quality of service", "reference" : "9.9.4.12", "presence" : "O", "format" : "TLV", "length" : "14-22"})
ies.append({ "iei" : "32", "value" : "Negotiated LLC SAPI", "type" : "LLC service access point identifier", "reference" : "9.9.4.7", "presence" : "O", "format" : "TV", "length" : "2"})
ies.append({ "iei" : "8-", "value" : "Radio priority", "type" : "Radio priority", "reference" : "9.9.4.13", "presence" : "O", "format" : "TV", "length" : "1"})
ies.append({ "iei" : "34", "value" : "Packet flow Identifier", "type" : "Packet flow Identifier", "reference" : "9.9.4.8", "presence" : "O", "format" : "TLV", "length" : "3"})
ies.append({ "iei" : "5E", "value" : "APN-AMBR", "type" : "APN aggregate maximum bit rate", "reference" : "9.9.4.2", "presence" : "O", "format" : "TLV", "length" : "4-8"})
ies.append({ "iei" : "27", "value" : "Protocol configuration options", "type" : "Protocol configuration options", "reference" : "9.9.4.11", "presence" : "O", "format" : "TLV", "length" : "3-253"})
ies.append({ "iei" : "C-", "value" : "WLAN offload indication", "type" : "WLAN offload acceptability", "reference" : "9.9.4.18", "presence" : "O", "format" : "TV", "length" : "1"})
ies.append({ "iei" : "33", "value" : "NBIFOM container", "type" : "NBIFOM container", "reference" : "9.9.4.19", "presence" : "O", "format" : "TLV", "length" : "3-257"})
ies.append({ "iei" : "66", "value" : "Header compression configuration", "type" : "Header compression configuration", "reference" : "9.9.4.22", "presence" : "O", "format" : "TLV", "length" : "5-257"})
ies.append({ "iei" : "7B", "value" : "Extended protocol configuration options", "type" : "Extended protocol configuration options", "reference" : "9.9.4.26", "presence" : "O", "format" : "TLV-E", "length" : "4-65538"})
msg_list[key]["ies"] = ies
