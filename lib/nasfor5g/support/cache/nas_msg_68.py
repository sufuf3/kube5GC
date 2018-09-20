ies = []
ies.append({ "iei" : "", "value" : "5GMM cause", "type" : "5GMM cause", "reference" : "9.10.3.2", "presence" : "M", "format" : "V", "length" : "1"})
ies.append({ "iei" : "5F", "value" : "T3346 value", "type" : "GPRS timer 2", "reference" : "9.10.2.4", "presence" : "O", "format" : "TLV", "length" : "3"})
ies.append({ "iei" : "16", "value" : "T3502 value", "type" : "GPRS timer 2", "reference" : "9.10.2.4", "presence" : "O", "format" : "TLV", "length" : "3"})
ies.append({ "iei" : "78", "value" : "EAP message", "type" : "EAP message", "reference" : "9.10.2.2", "presence" : "O", "format" : "TLV-E", "length" : "7-1503"})
msg_list[key]["ies"] = ies
