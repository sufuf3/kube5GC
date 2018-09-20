ies = []
ies.append({ "iei" : "", "value" : "De-registration type", "type" : "De-registration type", "reference" : "9.10.3.18", "presence" : "M", "format" : "V", "length" : "1/2"})
ies.append({ "iei" : "58", "value" : "5GMM cause", "type" : "5GMM cause", "reference" : "9.10.3.2", "presence" : "O", "format" : "TV", "length" : "2"})
ies.append({ "iei" : "5F", "value" : "T3346 value", "type" : "GPRS timer 2", "reference" : "9.10.2.4", "presence" : "O", "format" : "TLV", "length" : "3"})
msg_list[key]["ies"] = ies
