ies = []
ies.append({ "iei" : "59", "value" : "5GSM cause", "type" : "5GSM cause", "reference" : "9.10.4.2", "presence" : "O", "format" : "TV", "length" : "2"})
ies.append({ "iei" : "2A", "value" : "Session AMBR", "type" : "Session-AMBR", "reference" : "9.10.4.10", "presence" : "O", "format" : "TLV", "length" : "8"})
ies.append({ "iei" : "56", "value" : "RQ timer value", "type" : "GPRS timer", "reference" : "9.10.2.3", "presence" : "O", "format" : "TV", "length" : "2"})
ies.append({ "iei" : "7A", "value" : "Authorized QoS rules", "type" : "QoS rules", "reference" : "9.10.4.9", "presence" : "O", "format" : "TLV-E", "length" : "7-65538"})
ies.append({ "iei" : "7F", "value" : "Mapped EPS bearer contexts", "type" : "Mapped EPS bearer contexts", "reference" : "9.10.4.5", "presence" : "O", "format" : "TLV-E", "length" : "7-65538"})
ies.append({ "iei" : "7B", "value" : "Extended protocol configuration options", "type" : "Extended protocol configuration options", "reference" : "9.10.4.4", "presence" : "O", "format" : "TLV-E", "length" : "4-65538"})
msg_list[key]["ies"] = ies
