ies = []
ies.append({ "iei" : "", "value" : "Selected NAS security algorithms", "type" : "security algorithms", "reference" : "9.10.3.32", "presence" : "M", "format" : "V", "length" : "1"})
ies.append({ "iei" : "", "value" : "ngKSI", "type" : "key set identifier", "reference" : "9.10.3.29", "presence" : "M", "format" : "V", "length" : "1/2"})
ies.append({ "iei" : "", "value" : "Replayed UE security capabilities", "type" : "UE security capability", "reference" : "9.10.3.50", "presence" : "M", "format" : "LV", "length" : "3-5"})
ies.append({ "iei" : "E-", "value" : "IMEISV request", "type" : "IMEISV request", "reference" : "9.10.3.26", "presence" : "O", "format" : "TV", "length" : "1"})
ies.append({ "iei" : "4F", "value" : "HashAMF", "type" : "HashAMF", "reference" : "9.10.3.25", "presence" : "O", "format" : "TV", "length" : "9"})
ies.append({ "iei" : "57", "value" : "Selected EPS NAS security algorithms", "type" : "EPS NAS security algorithms", "reference" : "9.10.3.23", "presence" : "O", "format" : "TV", "length" : "2"})
ies.append({ "iei" : "78", "value" : "EAP message", "type" : "EAP message", "reference" : "9.10.2.2", "presence" : "O", "format" : "TLV-E", "length" : "7"})
msg_list[key]["ies"] = ies
