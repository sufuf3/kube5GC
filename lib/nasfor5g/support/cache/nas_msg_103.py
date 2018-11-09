ies = []
ies.append({ "iei" : "", "value" : "Payload container type", "type" : "Payload container type", "reference" : "9.10.3.36", "presence" : "M", "format" : "V", "length" : "1/2"})
ies.append({ "iei" : "", "value" : "Payload container", "type" : "Payload container", "reference" : "9.10.3.35", "presence" : "M", "format" : "LV-E", "length" : "3-65537"})
ies.append({ "iei" : "70", "value" : "PDU session ID", "type" : "PDU session identity 2", "reference" : "9.10.3.37", "presence" : "C", "format" : "TV", "length" : "2"})
ies.append({ "iei" : "61", "value" : "Old PDU session ID", "type" : "PDU session identity 2", "reference" : "9.10.3.37", "presence" : "O", "format" : "TV", "length" : "2"})
ies.append({ "iei" : "8-", "value" : "Request type", "type" : "Request type", "reference" : "9.10.3.43", "presence" : "O", "format" : "TV", "length" : "1"})
ies.append({ "iei" : "22", "value" : "S-NSSAI", "type" : "S-NSSAI", "reference" : "9.10.2.6", "presence" : "O", "format" : "TLV", "length" : "3-10"})
ies.append({ "iei" : "25", "value" : "DNN", "type" : "DNN", "reference" : "9.10.3.19", "presence" : "O", "format" : "TLV", "length" : "3-102"})
ies.append({ "iei" : "24", "value" : "Additional information", "type" : "Additional information", "reference" : "9.10.2.1", "presence" : "O", "format" : "TLV", "length" : "3-n"})
msg_list[key]["ies"] = ies
