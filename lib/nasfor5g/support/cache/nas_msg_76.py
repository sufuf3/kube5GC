ies = []
ies.append({ "iei" : "", "value" : "ngKSI", "type" : "key set identifier", "reference" : "9.10.3.29", "presence" : "M", "format" : "V", "length" : "1/2"})
ies.append({ "iei" : "", "value" : "5G-S-TMSI", "type" : "5GS mobile identity", "reference" : "9.10.3.4", "presence" : "M", "format" : "LV", "length" : "6"})
ies.append({ "iei" : "40", "value" : "Uplink data status", "type" : "Uplink data status", "reference" : "9.10.3.53", "presence" : "O", "format" : "TLV", "length" : "4-34"})
ies.append({ "iei" : "50", "value" : "PDU session status", "type" : "PDU session status", "reference" : "9.10.3.40", "presence" : "O", "format" : "TLV", "length" : "4-34"})
ies.append({ "iei" : "25", "value" : "Allowed PDU session status", "type" : "Allowed PDU session status", "reference" : "9.10.3.11", "presence" : "O", "format" : "TLV", "length" : "4-34"})
msg_list[key]["ies"] = ies
