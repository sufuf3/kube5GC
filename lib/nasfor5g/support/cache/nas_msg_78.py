ies = []
ies.append({ "iei" : "50", "value" : "PDU session status", "type" : "PDU session status", "reference" : "9.10.3.40", "presence" : "O", "format" : "TLV", "length" : "4-34"})
ies.append({ "iei" : "26", "value" : "PDU session reactivation result", "type" : "PDU session reactivation result", "reference" : "9.10.3.38", "presence" : "O", "format" : "TLV", "length" : "4-32"})
ies.append({ "iei" : "7E", "value" : "PDU session reactivation result error cause", "type" : "PDU session reactivation result error cause", "reference" : "9.10.3.39", "presence" : "O", "format" : "TLV-E", "length" : "5-515"})
ies.append({ "iei" : "78", "value" : "EAP message", "type" : "EAP message", "reference" : "9.10.2.2", "presence" : "O", "format" : "TLV-E", "length" : "7-1503"})
msg_list[key]["ies"] = ies
