ies = []
ies.append({ "iei" : "2D", "value" : "Authentication response parameter", "type" : "Authentication response parameter", "reference" : "9.10.3.15", "presence" : "O", "format" : "TLV", "length" : "6-18"})
ies.append({ "iei" : "78", "value" : "EAP message", "type" : "EAP message", "reference" : "9.10.2.2", "presence" : "O", "format" : "TLV-E", "length" : "7-1503"})
msg_list[key]["ies"] = ies
