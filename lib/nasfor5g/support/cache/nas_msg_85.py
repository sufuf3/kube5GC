ies = []
ies.append({ "iei" : "9-", "value" : "PDU session type", "type" : "PDU session type", "reference" : "9.10.4.8", "presence" : "O", "format" : "TV", "length" : "1"})
ies.append({ "iei" : "A-", "value" : "SSC mode", "type" : "SSC mode", "reference" : "9.10.4.12", "presence" : "O", "format" : "TV", "length" : "1"})
ies.append({ "iei" : "28", "value" : "5GSM capability", "type" : "5GSM capability", "reference" : "9.10.4.1", "presence" : "O", "format" : "TLV", "length" : "3-15"})
ies.append({ "iei" : "55", "value" : "Maximum number of supported packet filters", "type" : "Maximum number of supported packet filters", "reference" : "9.10.4.6", "presence" : "O", "format" : "TV", "length" : "3"})
ies.append({ "iei" : "TBD", "value" : "SM PDU DN request container", "type" : "SM PDU DN request container", "reference" : "9.10.4.11", "presence" : "O", "format" : "TBD", "length" : "TBD"})
ies.append({ "iei" : "7B", "value" : "Extended protocol configuration options", "type" : "Extended protocol configuration options", "reference" : "9.10.4.4", "presence" : "O", "format" : "TLV-E", "length" : "4-65538"})
msg_list[key]["ies"] = ies
