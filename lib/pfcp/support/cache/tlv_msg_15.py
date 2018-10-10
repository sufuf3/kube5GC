ies = []
ies.append({ "ie_type" : "Node ID", "ie_value" : "Node ID", "presence" : "M", "instance" : "0", "comment" : "This IE shall contain the unique identifier of the sending node."})
ies.append({ "ie_type" : "Cause", "ie_value" : "Cause", "presence" : "M", "instance" : "0", "comment" : "This IE shall indicate the acceptance or the rejection of the corresponding request message."})
ies.append({ "ie_type" : "Offending IE", "ie_value" : "Offending IE", "presence" : "C", "instance" : "0", "comment" : "This IE shall be included if the rejection is due to an conditional or mandatory IE missing or faulty."})
msg_list[key]["ies"] = ies
