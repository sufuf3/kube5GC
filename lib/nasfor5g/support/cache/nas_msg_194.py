ies = []
ies.append({ "iei" : "", "value" : "Selected PDU session type", "type" : "PDU session type", "reference" : "9.10.4.8", "presence" : "M", "format" : "V", "length" : "1/2"})
ies.append({ "iei" : "", "value" : "DNN", "type" : "DNN", "reference" : "9.10.3.19", "presence" : "M", "format" : "LV", "length" : "2-TBD"})
ies.append({ "iei" : "", "value" : "Authorized QoS rules", "type" : "QoS rules", "reference" : "9.10.4.9", "presence" : "M", "format" : "LV-E", "length" : "7-65538"})
ies.append({ "iei" : "", "value" : "Session AMBR", "type" : "Session-AMBR", "reference" : "9.10.4.10", "presence" : "M", "format" : "LV", "length" : "TBD"})
ies.append({ "iei" : "59", "value" : "5GSM cause", "type" : "5GSM cause", "reference" : "9.10.4.2", "presence" : "O", "format" : "TV", "length" : "2"})
ies.append({ "iei" : "29", "value" : "PDU address", "type" : "PDU address", "reference" : "9.10.4.7", "presence" : "O", "format" : "TLV", "length" : "7, 11 or 15"})
ies.append({ "iei" : "56", "value" : "RQ timer value", "type" : "GPRS timer", "reference" : "9.10.2.3", "presence" : "O", "format" : "TV", "length" : "2"})
ies.append({ "iei" : "22", "value" : "S-NSSAI", "type" : "S-NSSAI", "reference" : "9.10.2.6", "presence" : "O", "format" : "TLV", "length" : "3-10"})
ies.append({ "iei" : "7F", "value" : "Mapped EPS bearer contexts", "type" : "Mapped EPS bearer contexts", "reference" : "9.10.4.6", "presence" : "O", "format" : "TLV-E", "length" : "7-65538"})
ies.append({ "iei" : "78", "value" : "EAP message", "type" : "EAP message", "reference" : "9.10.2.2", "presence" : "O", "format" : "TLV-E", "length" : "7-1503"})
ies.append({ "iei" : "7B", "value" : "Extended protocol configuration options", "type" : "Extended protocol configuration options", "reference" : "9.10.4.4", "presence" : "O", "format" : "TLV-E", "length" : "4-65538"})
msg_list[key]["ies"] = ies
