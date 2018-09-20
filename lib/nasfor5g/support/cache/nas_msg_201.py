ies = []
ies.append({ "iei" : "28", "value" : "5GSM capability", "type" : "5GSM capability", "reference" : "9.10.4.1", "presence" : "O", "format" : "TLV", "length" : "3-15"})
ies.append({ "iei" : "55", "value" : "Maximum number of supported packet filters", "type" : "Maximum number of supported packet filters", "reference" : "9.10.4.6", "presence" : "O", "format" : "TV", "length" : "3"})
ies.append({ "iei" : "7A", "value" : "Requested QoS rules", "type" : "QoS rules", "reference" : "9.10.4.9", "presence" : "O", "format" : "TLV-E", "length" : "3-65538"})
ies.append({ "iei" : "7B", "value" : "Extended protocol configuration options", "type" : "Extended protocol configuration options", "reference" : "9.10.4.4", "presence" : "O", "format" : "TLV-E", "length" : "4-65538"})
msg_list[key]["ies"] = ies
