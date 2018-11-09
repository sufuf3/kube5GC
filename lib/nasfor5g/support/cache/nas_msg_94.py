ies = []
ies.append({ "iei" : "2C", "value" : "IMEISV", "type" : "5G mobile identity", "reference" : "9.10.3.4", "presence" : "O", "format" : "TLV", "length" : "11"})
ies.append({ "iei" : "7D", "value" : "NAS message container", "type" : "message container", "reference" : "9.10.3.31", "presence" : "O", "format" : "TLV-E", "length" : "3-n"})
msg_list[key]["ies"] = ies
