#
# Copyright (c) 2017, NextEPC Group
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
from docx import Document
import re, os, sys, string
import datetime
import getopt
import getpass

version = "0.1.0"

msg_list = {}
type_list = {}

verbosity = 0
filename = ""
outdir = './'
cachedir = './cache/'
currentdir = './'

FAIL = '\033[91m'
INFO = '\033[93m'
ENDC = '\033[0m'

def d_print(string):
    if verbosity > 0:
        sys.stdout.write(string)

def d_info(string):
    sys.stdout.write(INFO + string + ENDC + "\n")

def d_error(string):
    sys.stderr.write(FAIL + string + ENDC + "\n")
    sys.exit(0)

def write_file(f, string):
    f.write(string)
    d_print(string)

def output_header_to_file(f):
    now = datetime.datetime.now()
    f.write("""/*
 * Copyright (c) 2017, NextEPC Group
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

""")
    f.write("/*******************************************************************************\n")
    f.write(" * This file had been created by gtpv2c_tlv.py script v%s\n" % (version))
    f.write(" * Please do not modify this file but regenerate it via script.\n")
    f.write(" * Created on: %s by %s\n * from %s\n" % (str(now), getpass.getuser(), filename))
    f.write(" ******************************************************************************/\n\n")

def usage():
    print "Python generating NAS Message encoder/decoder v%s" % (version)
    print "Usage: python nas_message.py [options]"
    print "Available options:"
    print "-d        Enable script debug"
    print "-f [file] Input file to parse"
    print "-o [dir]  Output files to given directory"
    print "-c [dir]  Cache files to given directory"
    print "-h        Print this help and return"

def v_upper(v):
    return re.sub('_TO_UE', '', re.sub('_FROM_UE', '', re.sub('3GPP', '', re.sub('\'', '_', re.sub('/', '_', re.sub('-', '_', re.sub(' ', '_', v)))).upper())))

def v_lower(v):
    return re.sub('3gpp', '', re.sub('\'', '_', re.sub('/', '_', re.sub('-', '_', re.sub(' ', '_', v)))).lower())

def get_cells(cells):
    iei = cells[0].text.encode('ascii', 'ignore')
    value = re.sub("\s*$", "", re.sub("\s*\n*\s*\([^\)]*\)*", "", re.sub("'s", "", cells[1].text))).encode('ascii', 'ignore')
    type = re.sub("^NAS ", "", re.sub("'s", "", re.sub('\s*\n\s*[a-zA-Z0-9.]*', '', cells[2].text))).encode('ascii', 'ignore')
    reference = re.sub('[a-zA-Z0-9\'\-\s]*\n\s*', '', cells[2].text).encode('ascii', 'ignore')
    presence = cells[3].text.encode('ascii', 'ignore')
    format = cells[4].text.encode('ascii', 'ignore')
    length = cells[5].text.encode('ascii', 'ignore')

    return { "iei" : iei, "value" : value, "type" : type, "reference" : reference, "presence" : presence, "format" : format, "length" : length }

def write_cells_to_file(name, cells):
    write_file(f, name + ".append({ \"iei\" : \"" + cells["iei"] + \
        "\", \"value\" : \"" + cells["value"] + \
        "\", \"type\" : \"" + cells["type"] + \
        "\", \"reference\" : \"" + cells["reference"] + \
        "\", \"presence\" : \"" + cells["presence"] + \
        "\", \"format\" : \"" + cells["format"] + \
        "\", \"length\" : \"" + cells["length"] + "\"})\n")

try:
    opts, args = getopt.getopt(sys.argv[1:], "df:ho:c:", ["debug", "file", "help", "output", "cache"])
except getopt.GetoptError as err:
    # print help information and exit:
    usage()
    sys.exit(2)

for o, a in opts:
    if o in ("-d", "--debug"):
        verbosity = 1
    if o in ("-f", "--file"):
        filename = a
    if o in ("-o", "--output"):
        outdir = a
        if outdir.rfind('/') != len(outdir):
            outdir += '/'
    if o in ("-c", "--cache"):
        cache = a
        if cachedir.rfind('/') != len(cachedir):
            cachedir += '/'
    if o in ("-h", "--help"):
        usage()
        sys.exit(2)
       
#############reference from TS24.501-f00 9.7.1###############

# Message Type List
msg_list["REGISTRATION REQUEST"] = { "type" : "65" }
msg_list["REGISTRATION ACCEPT"]= { "type" : "66" }
msg_list["REGISTRATION COMPLETE"] = { "type" : "67" }
msg_list["REGISTRATION REJECT"] = { "type" : "68" }
msg_list["DEREGISTRATION REQUEST FROM UE"] = { "type" : "69" }
msg_list["DEREGISTRATION ACCEPT FROM UE"] = { "type" : "70" }
msg_list["DEREGISTRATION REQUEST TO UE"] = { "type" : "71" }
msg_list["DEREGISTRATION ACCEPT TO UE"] = { "type" : "72" }
msg_list["SERVICE REQUEST"] = { "type" : "76" }
msg_list["SERVICE REJECT"] = { "type" : "77" }
msg_list["SERVICE ACCEPT"] = { "type" : "78" }
msg_list["CONFIGURATION UPDATE COMMAND"] = { "type" : "84" }
msg_list["CONFIGURATION UPDATE COMPLETE"] = { "type" : "85" }
msg_list["AUTHENTICATION REQUEST"] = { "type" : "86" }
msg_list["AUTHENTICATION RESPONSE"] = { "type" : "87" }
msg_list["AUTHENTICATION REJECT"] = { "type" : "88" }
msg_list["AUTHENTICATION FAILURE"] = { "type" : "89" }
msg_list["AUTHENTICATION RESULT"] = { "type" : "90" }
msg_list["IDENTITY REQUEST"] = { "type" : "91" }
msg_list["IDENTITY RESPONSE"] = { "type" : "92" }
msg_list["SECURITY MODE COMMAND"] = { "type" : "93" }
msg_list["SECURITY MODE COMPLETE"] = { "type" : "94" }
msg_list["SECURITY MODE REJECT"] = { "type" : "95" }
msg_list["GMM STATUS"] = { "type" : "100" }
msg_list["NOTIFICATION"] = { "type" : "101" }
msg_list["NOTIFICATION RESPONSE"] = { "type" : "102" }
msg_list["UL NAS TRANSPORT"] = { "type" : "103" }
msg_list["DL NAS TRANSPORT"] = { "type" : "104" }


msg_list["PDU SESSION ESTABLISHMENT REQUEST"] = { "type" : "193" }
msg_list["PDU SESSION ESTABLISHMENT ACCEPT"] = { "type" : "194" }
msg_list["PDU SESSION ESTABLISHMENT REJECT"] = { "type" : "195" }
msg_list["PDU SESSION AUTHENTICATION COMMAND"] = { "type" : "197" }
msg_list["PDU SESSION AUTHENTICATION COMPLETE"] = { "type" : "198" }
msg_list["PDU SESSION AUTHENTICATION RESULT"] = { "type" : "199" }
msg_list["PDU SESSION MODIFICATION REQUEST"] = { "type" : "201" }
msg_list["PDU SESSION MODIFICATION REJECT"] = { "type" : "202" }
msg_list["PDU SESSION MODIFICATION COMMAND"] = { "type" : "203" }
msg_list["PDU SESSION MODIFICATION COMPLETE"] = { "type" : "204" }
msg_list["PDU SESSION MODIFICATION COMMAND REJECT"] = { "type" : "205" }
msg_list["PDU SESSION RELEASE REQUEST"] = { "type" : "209" }
msg_list["PDU SESSION RELEASE REJECT"] = { "type" : "210" }
msg_list["PDU SESSION RELEASE COMMAND"] = { "type" : "211" }
msg_list["PDU SESSION RELEASE COMPLETE"] = { "type" : "212" }
msg_list["GSM STATUS"] = { "type" : "214" }

#############reference from TS24.501-f00 Chapter 8###############

# Table number for Message List
msg_list["AUTHENTICATION REQUEST"]["table"] = 0
msg_list["AUTHENTICATION RESPONSE"]["table"] = 1
msg_list["AUTHENTICATION RESULT"]["table"] = 2
msg_list["AUTHENTICATION FAILURE"]["table"] = 3
msg_list["AUTHENTICATION REJECT"]["table"] = 4
msg_list["REGISTRATION REQUEST"]["table"] = 5
msg_list["REGISTRATION ACCEPT"]["table"] = 6
msg_list["REGISTRATION COMPLETE"]["table"] = 7
msg_list["REGISTRATION REJECT"]["table"] = 8
msg_list["UL NAS TRANSPORT"]["table"] = 9
msg_list["DL NAS TRANSPORT"]["table"] = 10
msg_list["DEREGISTRATION REQUEST FROM UE"]["table"] = 11
msg_list["DEREGISTRATION ACCEPT FROM UE"]["table"] = 12
msg_list["DEREGISTRATION REQUEST TO UE"]["table"] = 13
msg_list["DEREGISTRATION ACCEPT TO UE"]["table"] = 14
msg_list["SERVICE REQUEST"]["table"] = 15
msg_list["SERVICE ACCEPT"]["table"] = 16
msg_list["SERVICE REJECT"]["table"] = 17
msg_list["CONFIGURATION UPDATE COMMAND"]["table"] = 18
msg_list["CONFIGURATION UPDATE COMPLETE"]["table"] = 29
msg_list["IDENTITY REQUEST"]["table"] = 20
msg_list["IDENTITY RESPONSE"]["table"] = 21
msg_list["NOTIFICATION"]["table"] = 22
msg_list["NOTIFICATION RESPONSE"]["table"] = 23
msg_list["SECURITY MODE COMMAND"]["table"] = 24
msg_list["SECURITY MODE COMPLETE"]["table"] = 25
msg_list["SECURITY MODE REJECT"]["table"] = 26
msg_list["GMM STATUS"]["table"] = 28

msg_list["PDU SESSION ESTABLISHMENT REQUEST"]["table"] = 29
msg_list["PDU SESSION ESTABLISHMENT ACCEPT"]["table"] = 30
msg_list["PDU SESSION ESTABLISHMENT REJECT"]["table"] = 31
msg_list["PDU SESSION AUTHENTICATION COMMAND"]["table"] = 32
msg_list["PDU SESSION AUTHENTICATION COMPLETE"]["table"] = 33
msg_list["PDU SESSION AUTHENTICATION RESULT"]["table"] = 34
msg_list["PDU SESSION MODIFICATION REQUEST"]["table"] = 35
msg_list["PDU SESSION MODIFICATION REJECT"]["table"] = 36
msg_list["PDU SESSION MODIFICATION COMMAND"]["table"] = 37
msg_list["PDU SESSION MODIFICATION COMPLETE"]["table"] = 38
msg_list["PDU SESSION MODIFICATION COMMAND REJECT"]["table"] = 39
msg_list["PDU SESSION RELEASE REQUEST"]["table"] = 40
msg_list["PDU SESSION RELEASE REJECT"]["table"] = 41
msg_list["PDU SESSION RELEASE COMMAND"]["table"] = 42
msg_list["PDU SESSION RELEASE COMPLETE"]["table"] = 43
msg_list["GSM STATUS"]["table"] = 44

for key in msg_list.keys():
    if "table" not in msg_list[key].keys():
        continue

    d_info("[" + key + "]")
    cachefile = cachedir + "nas_msg_" + msg_list[key]["type"] + ".py"
    if os.path.isfile(cachefile) and os.access(cachefile, os.R_OK):
        execfile(cachefile)
        print "Read from " + cachefile
    else:
        document = Document(filename)
        f = open(cachefile, 'w') 

        ies = []
        write_file(f, "ies = []\n")
        table = document.tables[msg_list[key]["table"]]

        start_row = 0
        for start_row, row in enumerate(table.rows):
            cells = get_cells(row.cells);
            if cells["type"].find('Message type') != -1:
                break
            if cells["type"].find('KSI and sequence number') != -1:
                start_row -= 1
                break

        assert start_row <= 4, "Can't find message type"

        half_length = True;
        for row in table.rows[start_row+1:]:
            cells = get_cells(row.cells)
            if cells is None:
                continue

            if cells["length"] == "1/2":
                if half_length is True:
                    half_length = False;
                else:
                    half_length = True;
                    continue;

            ies.append(cells)
            write_cells_to_file("ies", cells)

        msg_list[key]["ies"] = ies
        write_file(f, "msg_list[key][\"ies\"] = ies\n")

        f.close()


tmp = [(k, v["type"]) for k, v in msg_list.items()]
sorted_msg_list = sorted(tmp, key=lambda tup: float(tup[1]))

for (k, v) in sorted_msg_list:
    if "ies" not in msg_list[k]:
        continue;

    for ie in msg_list[k]["ies"]:
        key = ie["type"]
        if key in type_list.keys() and (type_list[key]["presence"] != ie["presence"] or type_list[key]["format"] != ie["format"] or type_list[key]["length"] != ie["length"]):
            d_print("KEY type different : %s\n" % key)
            d_print("%s.%s %s %s %s\n" % (v_lower(type_list[key]["message"]), type_list[key]["value"], type_list[key]["presence"], type_list[key]["format"], type_list[key]["length"]))
            d_print("%s.%s %s %s %s\n\n" % (v_lower(k), ie["value"], ie["presence"], ie["format"], ie["length"]))
            continue
        type_list[key] = { "reference" : ie["reference"], "presence" : ie["presence"], "format" : ie["format"], "length" : ie["length"], "message" : k, "value" : ie["value"] }

d_info("[Type List]")
typefile = currentdir + "type_list.py"
if os.path.isfile(typefile) and os.access(typefile, os.R_OK):
    execfile(typefile)
    print "Read from " + typefile

tmp = [(k, v["reference"]) for k, v in type_list.items()]
sorted_type_list = sorted(tmp, key=lambda tup: tup[1])

f = open(outdir + 'nas_ies.h', 'w')
output_header_to_file(f)

f.write("""#ifndef __NAS_IES_H__
#define __NAS_IES_H__

#include "core_pkbuf.h"
#include "nas_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CORE_DECLARE(c_int16_t) nas_encode_optional_type(pkbuf_t *pkbuf, c_uint8_t type);

""")

for (k, v) in sorted_type_list:
    f.write("CORE_DECLARE(c_int16_t) nas_decode_%s(nas_%s_t *%s, pkbuf_t *pkbuf);\n" % (v_lower(k), v_lower(k), v_lower(k)))
f.write("\n")

for (k, v) in sorted_type_list:
    f.write("CORE_DECLARE(c_int16_t) nas_encode_%s(pkbuf_t *pkbuf, nas_%s_t *%s);\n" % (v_lower(k), v_lower(k), v_lower(k)))
f.write("\n")


f.write("""#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NAS_IES_H__ */

""")
f.close()

f = open(outdir + 'nas_ies.c', 'w')
output_header_to_file(f)
f.write("""#define TRACE_MODULE _nas_ies

#include "core_debug.h"
#include "core_lib.h"
#include "nas_ies.h"

c_int16_t nas_encode_optional_type(pkbuf_t *pkbuf, c_uint8_t type)
{
    c_uint16_t size = sizeof(c_uint8_t);

    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, "pkbuf_header error");
    memcpy(pkbuf->payload - size, &type, size);

    return size;
}
""")

for (k, v) in sorted_type_list:
#    d_print("%s = %s\n" % (k, type_list[k]))
    f.write("/* %s %s\n" % (type_list[k]["reference"], k))
    f.write(" * %s %s %s */\n" % (type_list[k]["presence"], type_list[k]["format"], type_list[k]["length"]))
    if type_list[k]["format"] == "TV" and type_list[k]["length"] == "1":
        f.write("c_int16_t nas_decode_%s(nas_%s_t *%s, pkbuf_t *pkbuf)\n" % (v_lower(k), v_lower(k), v_lower(k)))
        f.write("{\n")
        f.write("    memcpy(%s, pkbuf->payload - 1, 1);\n\n" % v_lower(k))
        f.write("    d_trace(25, \"  %s - \");\n" % v_upper(k))
        f.write("    d_trace_hex(25, pkbuf->payload - 1, 1);\n\n");
        f.write("    return 0;\n")
        f.write("}\n\n")
        f.write("c_int16_t nas_encode_%s(pkbuf_t *pkbuf, nas_%s_t *%s)\n" % (v_lower(k), v_lower(k), v_lower(k)))
        f.write("{\n")
        f.write("    c_uint16_t size = sizeof(nas_%s_t);\n\n" % v_lower(k))
        f.write("    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, \"pkbuf_header error\");\n")
        f.write("    memcpy(pkbuf->payload - size, %s, size);\n\n" % v_lower(k))
        f.write("    d_trace(25, \"  %s - \");\n" % v_upper(k))
        f.write("    d_trace_hex(25, pkbuf->payload - size, size);\n\n");
        f.write("    return size;\n")
        f.write("}\n\n")
    elif type_list[k]["format"] == "TV" or type_list[k]["format"] == "V":
        f.write("c_int16_t nas_decode_%s(nas_%s_t *%s, pkbuf_t *pkbuf)\n" % (v_lower(k), v_lower(k), v_lower(k)))
        f.write("{\n")
        if type_list[k]["length"] == "4":
            f.write("    c_uint16_t size = 3;\n\n")
        else:
            f.write("    c_uint16_t size = sizeof(nas_%s_t);\n\n" % v_lower(k))
        f.write("    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, \"pkbuf_header error\");\n")
        f.write("    memcpy(%s, pkbuf->payload - size, size);\n\n" % v_lower(k))
        if "decode" in type_list[k]:
            f.write("%s" % type_list[k]["decode"])
        f.write("    d_trace(25, \"  %s - \");\n" % v_upper(k))
        f.write("    d_trace_hex(25, pkbuf->payload - size, size);\n\n");
        f.write("    return size;\n")
        f.write("}\n\n")
        f.write("c_int16_t nas_encode_%s(pkbuf_t *pkbuf, nas_%s_t *%s)\n" % (v_lower(k), v_lower(k), v_lower(k)))
        f.write("{\n")
        if type_list[k]["length"] == "4":
            f.write("    c_uint16_t size = 3;\n")
        else:
            f.write("    c_uint16_t size = sizeof(nas_%s_t);\n" % v_lower(k))
        f.write("    nas_%s_t target;\n\n" % v_lower(k))
        f.write("    memcpy(&target, %s, size);\n" % v_lower(k))
        if "encode" in type_list[k]:
            f.write("%s" % type_list[k]["encode"])
        f.write("    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, \"pkbuf_header error\");\n")
        f.write("    memcpy(pkbuf->payload - size, &target, size);\n\n")
        f.write("    d_trace(25, \"  %s - \");\n" % v_upper(k))
        f.write("    d_trace_hex(25, pkbuf->payload - size, size);\n\n");
        f.write("    return size;\n")
        f.write("}\n\n")
    elif type_list[k]["format"] == "LV-E" or type_list[k]["format"] == "TLV-E":
        f.write("c_int16_t nas_decode_%s(nas_%s_t *%s, pkbuf_t *pkbuf)\n" % (v_lower(k), v_lower(k), v_lower(k)))
        f.write("{\n")
        f.write("    c_uint16_t size = 0;\n")
        f.write("    nas_%s_t *source = pkbuf->payload;\n\n" % v_lower(k))
        f.write("    %s->length = ntohs(source->length);\n" % v_lower(k))
        f.write("    size = %s->length + sizeof(%s->length);\n\n" % (v_lower(k), v_lower(k)))
        f.write("    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, \"pkbuf_header error\");\n")
        f.write("    %s->buffer = pkbuf->payload - size + sizeof(%s->length);\n\n" % (v_lower(k), v_lower(k)))
        f.write("    d_trace(25, \"  %s - \");\n" % v_upper(k))
        f.write("    d_trace_hex(25, %s->buffer, %s->length);\n\n" % (v_lower(k), v_lower(k)));
        f.write("    return size;\n")
        f.write("}\n\n")
        f.write("c_int16_t nas_encode_%s(pkbuf_t *pkbuf, nas_%s_t *%s)\n" % (v_lower(k), v_lower(k), v_lower(k)))
        f.write("{\n")
        f.write("    c_uint16_t size = 0;\n")
        f.write("    c_uint16_t target;\n\n")
        f.write("    d_assert(%s, return -1, \"Null param\");\n" % v_lower(k))
        f.write("    d_assert(%s->buffer, return -1, \"Null param\");\n\n" % v_lower(k))
        f.write("    size = sizeof(%s->length);\n" % v_lower(k))
        f.write("    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, \"pkbuf_header error\");\n")
        f.write("    target = htons(%s->length);\n" % v_lower(k))
        f.write("    memcpy(pkbuf->payload - size, &target, size);\n\n")
        f.write("    size = %s->length;\n" % v_lower(k))
        f.write("    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, \"pkbuf_header error\");\n")
        f.write("    memcpy(pkbuf->payload - size, %s->buffer, size);\n\n" % v_lower(k))
        f.write("    d_trace(25, \"  %s - \");\n" % v_upper(k))
        f.write("    d_trace_hex(25, pkbuf->payload - size, size);\n\n");
        f.write("    return %s->length + sizeof(%s->length);\n" % (v_lower(k), v_lower(k)))
        f.write("}\n\n");
    else:
        f.write("c_int16_t nas_decode_%s(nas_%s_t *%s, pkbuf_t *pkbuf)\n" % (v_lower(k), v_lower(k), v_lower(k)))
        f.write("{\n")
        f.write("    c_uint16_t size = 0;\n")
        f.write("    nas_%s_t *source = pkbuf->payload;\n\n" % v_lower(k))
        f.write("    %s->length = source->length;\n" % v_lower(k))
        f.write("    size = %s->length + sizeof(%s->length);\n\n" % (v_lower(k), v_lower(k)))
        f.write("    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, \"pkbuf_header error\");\n")
        f.write("    memcpy(%s, pkbuf->payload - size, size);\n\n" % v_lower(k))
        if "decode" in type_list[k]:
            f.write("%s" % type_list[k]["decode"])
        f.write("    d_trace(25, \"  %s - \");\n" % v_upper(k))
        f.write("    d_trace_hex(25, pkbuf->payload - size, size);\n\n");
        f.write("    return size;\n")
        f.write("}\n\n")
        f.write("c_int16_t nas_encode_%s(pkbuf_t *pkbuf, nas_%s_t *%s)\n" % (v_lower(k), v_lower(k), v_lower(k)))
        f.write("{\n")
        f.write("    c_uint16_t size = %s->length + sizeof(%s->length);\n" % (v_lower(k), v_lower(k)))
        f.write("    nas_%s_t target;\n\n" % v_lower(k))
        f.write("    memcpy(&target, %s, sizeof(nas_%s_t));\n" % (v_lower(k), v_lower(k)))
        if "encode" in type_list[k]:
            f.write("%s" % type_list[k]["encode"])
        f.write("    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, \"pkbuf_header error\");\n")
        f.write("    memcpy(pkbuf->payload - size, &target, size);\n\n")
        f.write("    d_trace(25, \"  %s - \");\n" % v_upper(k))
        f.write("    d_trace_hex(25, pkbuf->payload - size, size);\n\n");
        f.write("    return size;\n")
        f.write("}\n\n");
f.close()

f = open(outdir + 'nas_message.h', 'w')
output_header_to_file(f)
f.write("""#ifndef __NAS_MESSAGE_H__
#define __NAS_MESSAGE_H__

#include "nas_ies.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* The Packet Buffer(pkbuf_t) for NAS message MUST make a HEADROOM. 
 * When calculating AES_CMAC, we need to use the headroom of the packet. */
#define NAS_HEADROOM 16

/***reference from TS24.007-f20 9.3.1******/

#define NAS_SECURITY_HEADER_PLAIN_NAS_MESSAGE 0
#define NAS_SECURITY_HEADER_INTEGRITY_PROTECTED 1
#define NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_CIPHERED 2
#define NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_NEW_SECURITY_CONTEXT 3
#define NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_CIPHTERD_WITH_NEW_INTEGRITY_CONTEXT 4

/***reference from TS24.007-f20 11.2.3.1**/
#define NAS_EXTENDED_PROTOCOL_DISCRIMINATOR_GSM 0x2e
#define NAS_EXTENDED_PROTOCOL_DISCRIMINATOR_GMM 0x7e

#define NAS_PDU_SESSION_IDENTITY_UNASSIGNED 0
#define NAS_PROCEDURE_TRANSACTION_IDENTITY_UNASSIGNED 0

typedef struct _nas_gmm_header_t {
    c_uint8_t extended_protocol_discriminator;
    c_uint8_t security_header_type;
    c_uint8_t message_type;
} __attribute__ ((packed)) nas_gmm_header_t;

typedef struct _nas_gsm_header_t {
    c_uint8_t extended_protocol_discriminator;
    c_uint8_t pdu_session_identity;
    c_uint8_t procedure_transaction_identity;
    c_uint8_t message_type;
} __attribute__ ((packed)) nas_gsm_header_t;

typedef struct _nas_security_header_t {
    c_uint8_t extended_protocol_discriminator;
    c_uint8_t security_header_type;
    c_uint32_t message_authentication_code;
    c_uint8_t sequence_number;
} __attribute__ ((packed)) nas_security_header_t;

""")

for (k, v) in sorted_msg_list:
    if k.find("TO UE") == -1 and k != "SERVICE REQUEST":
        f.write("#define NAS_" + v_upper(k) + " " + v.split('.')[0] + "\n")
f.write("\n")

for (k, v) in sorted_msg_list:
    if "ies" not in msg_list[k]:
        continue;
    if len(msg_list[k]["ies"]) == 0:
        continue;

    f.write("\n/*******************************************************\n")
    f.write(" * %s\n" % k)
    f.write(" ******************************************************/")

    for i, ie in enumerate([ies for ies in msg_list[k]["ies"] if ies["presence"] == "O"]):
        f.write("\n#define NAS_%s_%s_PRESENT (1<<%d)" % (v_upper(k), v_upper(ie["value"]), i))

    for i, ie in enumerate([ies for ies in msg_list[k]["ies"] if ies["presence"] == "O"]):
        f.write("\n#define NAS_%s_%s_TYPE 0x%s" % (v_upper(k), v_upper(ie["value"]), re.sub('-', '0', ie["iei"])))

    f.write("\n\ntypedef struct _nas_%s_t {\n" % v_lower(k))

    mandatory_fields = False;
    optional_fields = False;
    for ie in msg_list[k]["ies"]:
        if ie["presence"] == "M" and mandatory_fields is False:
            f.write("    /* Mandatory fields */\n")
            mandatory_fields = True;

        if ie["presence"] == "O" and optional_fields is False:
            f.write("\n    /* Optional fields */\n")
            f.write("    c_uint32_t presencemask;\n");
            optional_fields = True;

        f.write("    nas_" + v_lower(ie["type"]) + "_t " + \
                v_lower(ie["value"]) + ";\n")

    f.write("} nas_%s_t;\n\n" % v_lower(k))

f.write("\n")

f.write("""typedef struct _nas_gmm_message_t {
    nas_gmm_header_t h;
    union {
""")
for (k, v) in sorted_msg_list:
    if "ies" not in msg_list[k]:
        continue;
    if len(msg_list[k]["ies"]) == 0:
        continue;
    if float(msg_list[k]["type"]) < 192:
        f.write("        nas_%s_t %s;\n" % (v_lower(k), v_lower(k)))
f.write("""    };
} nas_gmm_message_t;

typedef struct _nas_gsm_message_t {
    nas_gsm_header_t h;
    union {
""")
for (k, v) in sorted_msg_list:
    if "ies" not in msg_list[k]:
        continue;
    if len(msg_list[k]["ies"]) == 0:
        continue;
    if float(msg_list[k]["type"]) >= 192:
        f.write("        nas_%s_t %s;\n" % (v_lower(k), v_lower(k)))

f.write("""    };
} nas_gsm_message_t;

typedef struct _nas_message_t {
    nas_security_header_t h;
    union {
        nas_gmm_message_t gmm;
        nas_gsm_message_t gsm;
    };
} nas_message_t;

CORE_DECLARE(status_t) nas_gmm_decode(nas_message_t *message, pkbuf_t *pkbuf);
CORE_DECLARE(status_t) nas_gsm_decode(nas_message_t *message, pkbuf_t *pkbuf);
CORE_DECLARE(status_t) nas_plain_encode(
        pkbuf_t **pkbuf, nas_message_t *message);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __NAS_MESSAGE_H__ */
""")

f.close()



f = open(outdir + 'nas_decoder.c', 'w')
output_header_to_file(f)
f.write("""#define TRACE_MODULE _nas_decoder

#include "core_debug.h"
#include "nas_message.h"

""")

for (k, v) in sorted_msg_list:
    if "ies" not in msg_list[k]:
        continue
    if len(msg_list[k]["ies"]) == 0:
        continue

    f.write("c_int32_t nas_decode_%s(nas_message_t *message, pkbuf_t *pkbuf)\n{\n" % v_lower(k))
    if float(msg_list[k]["type"]) < 192:
        f.write("    nas_%s_t *%s = &message->gmm.%s;\n" % (v_lower(k), v_lower(k), v_lower(k)))
    else:
        f.write("    nas_%s_t *%s = &message->gsm.%s;\n" % (v_lower(k), v_lower(k), v_lower(k)))
    f.write("    c_int32_t decoded = 0;\n")
    f.write("    c_int32_t size = 0;\n\n")
    f.write("    d_trace(25, \"[NAS] Decode %s\\n\");\n\n" % v_upper(k))

    for ie in [ies for ies in msg_list[k]["ies"] if ies["presence"] == "M"]:
        f.write("    size = nas_decode_%s(&%s->%s, pkbuf);\n" % (v_lower(ie["type"]), v_lower(k), v_lower(ie["value"])))
        f.write("    d_assert(size >= 0, return -1, \"decode failed\");\n")
        f.write("    decoded += size;\n\n")

    optional_fields = False;
    for ie in [ies for ies in msg_list[k]["ies"] if ies["presence"] == "O"]:
        if optional_fields is False:
            f.write("""    while(pkbuf->len > 0) 
    {
        c_uint8_t *buffer = pkbuf->payload;
        c_uint8_t type = (*buffer) >= 0x80 ? ((*buffer) & 0xf0) : (*buffer);

        size = sizeof(c_uint8_t);
        d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, return -1, 
                "pkbuf_header error");
        decoded += size;

        switch(type)
        {
""")
            optional_fields = True;

        f.write("             case NAS_%s_%s_TYPE:\n" % (v_upper(k), v_upper(ie["value"])))
        f.write("                 size = nas_decode_%s(&%s->%s, pkbuf);\n" % (v_lower(ie["type"]), v_lower(k), v_lower(ie["value"])))
        f.write("                 d_assert(size >= 0, return -1, \"decode failed\");\n")
        f.write("                 %s->presencemask |= NAS_%s_%s_PRESENT;\n" % (v_lower(k), v_upper(k), v_upper(ie["value"])))
        f.write("                 decoded += size;\n")
        f.write("                 break;\n")

    if [ies for ies in msg_list[k]["ies"] if ies["presence"] == "O"]:
        f.write("""             default:
                d_warn("Unknown type(0x%x) or not implemented\\n", type);
                break;
        }
    }

""")
    f.write("""    return decoded;
}

""")

f.write("""status_t nas_gmm_decode(nas_message_t *message, pkbuf_t *pkbuf)
{
    status_t rv = CORE_ERROR;
    c_uint16_t size = 0;
    c_uint16_t decoded = 0;

    d_assert(pkbuf, return CORE_ERROR, "Null param");
    d_assert(pkbuf->payload, return CORE_ERROR, "Null param");

    memset(message, 0, sizeof(nas_message_t));

    size = sizeof(nas_gmm_header_t);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, 
            return CORE_ERROR, "pkbuf_header error");
    memcpy(&message->gmm.h, pkbuf->payload - size, size);
    decoded += size;

    if (message->gmm.h.security_header_type >=
            NAS_SECURITY_HEADER_FOR_SERVICE_REQUEST_MESSAGE)
    {
        d_assert(pkbuf_header(pkbuf, 1) == CORE_OK, 
                return CORE_ERROR, "pkbuf_header error");
        decoded -= 1;
        size = nas_decode_service_request(message, pkbuf);
        d_assert(size >= CORE_OK, return CORE_ERROR, "decode error");
        decoded += size;

        goto out;
    }

    switch(message->gmm.h.message_type)
    {
""")
for (k, v) in sorted_msg_list:
    if "ies" not in msg_list[k]:
        continue;
    if float(msg_list[k]["type"]) < 192 and k.find("TO UE") == -1 and k != "SERVICE REQUEST":
        f.write("        case NAS_%s:\n" % v_upper(k))
        if len(msg_list[k]["ies"]) != 0:
            f.write("            size = nas_decode_%s(message, pkbuf);\n" % v_lower(k))
            f.write("            d_assert(size >= CORE_OK, return CORE_ERROR, \"decode error\");\n")
            f.write("            decoded += size;\n")
        f.write("            break;\n")

f.write("""        default:
            d_error("Unknown message type (0x%x) or not implemented", 
                    message->gmm.h.message_type);
            break;
    }

out:
    rv = pkbuf_header(pkbuf, decoded);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pkbuf_header error");

    return CORE_OK;
}
""")

f.write("""status_t nas_gsm_decode(nas_message_t *message, pkbuf_t *pkbuf)
{
    status_t rv = CORE_ERROR;
    c_uint16_t size = 0;
    c_uint16_t decoded = 0;

    d_assert(pkbuf, return CORE_ERROR, "Null param");
    d_assert(pkbuf->payload, return CORE_ERROR, "Null param");

    memset(message, 0, sizeof(nas_message_t));

    size = sizeof(nas_gsm_header_t);
    d_assert(pkbuf_header(pkbuf, -size) == CORE_OK, 
            return CORE_ERROR, "pkbuf_header error");
    memcpy(&message->gsm.h, pkbuf->payload - size, size);
    decoded += size;

    switch(message->gsm.h.message_type)
    {
""")
for (k, v) in sorted_msg_list:
    if "ies" not in msg_list[k]:
        continue;
    if float(msg_list[k]["type"]) >= 192:
        f.write("        case NAS_%s:\n" % v_upper(k))
        if len(msg_list[k]["ies"]) != 0:
            f.write("            size = nas_decode_%s(message, pkbuf);\n" % v_lower(k))
            f.write("            d_assert(size >= CORE_OK, return CORE_ERROR, \"decode error\");\n")
            f.write("            decoded += size;\n")
        f.write("            break;\n")

f.write("""        default:
            d_error("Unknown message type (0x%x) or not implemented", 
                    message->gsm.h.message_type);
            break;
    }

    rv = pkbuf_header(pkbuf, decoded);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pkbuf_header error");

    return CORE_OK;
}

#if 0 /* deprecated */
status_t nas_plain_decode(nas_message_t *message, pkbuf_t *pkbuf)
{
    nas_security_header_t *h = NULL;

    d_assert(pkbuf, return CORE_ERROR, "Null param");
    h = pkbuf->payload;
    d_assert(h, return CORE_ERROR, "Null param");

    if (h->extended_protocol_discriminator == NAS_EXTENDED_PROTOCOL_DISCRIMINATOR_GMM)
        return nas_gmm_decode(message, pkbuf);
    else if (h->extended_protocol_discriminator == NAS_EXTENDED_PROTOCOL_DISCRIMINATOR_GSM)
        return nas_gsm_decode(message, pkbuf);

    d_assert(0, return CORE_ERROR, 
            "Invalid Protocol : %d", h->extended_protocol_discriminator);
}
#endif
""")

f.close()

f = open(outdir + 'nas_encoder.c', 'w')
output_header_to_file(f)
f.write("""#define TRACE_MODULE _nas_encoder

#include "core_debug.h"
#include "nas_message.h"

""")

for (k, v) in sorted_msg_list:
    if "ies" not in msg_list[k]:
        continue;
    if len(msg_list[k]["ies"]) == 0:
        continue

    f.write("c_int32_t nas_encode_%s(pkbuf_t *pkbuf, nas_message_t *message)\n{\n" % v_lower(k))
    if float(msg_list[k]["type"]) < 192:
        f.write("    nas_%s_t *%s = &message->gmm.%s;\n" % (v_lower(k), v_lower(k), v_lower(k)))
    else:
        f.write("    nas_%s_t *%s = &message->gsm.%s;\n" % (v_lower(k), v_lower(k), v_lower(k)))
    f.write("    c_int32_t encoded = 0;\n")
    f.write("    c_int32_t size = 0;\n\n")
    f.write("    d_trace(25, \"[NAS] Encode %s\\n\");\n\n" % v_upper(k))

    for ie in [ies for ies in msg_list[k]["ies"] if ies["presence"] == "M"]:
        f.write("    size = nas_encode_%s(pkbuf, &%s->%s);\n" % (v_lower(ie["type"]), v_lower(k), v_lower(ie["value"])))
        f.write("    d_assert(size >= 0, return -1, \"encode failed\");\n")
        f.write("    encoded += size;\n\n")

    for ie in [ies for ies in msg_list[k]["ies"] if ies["presence"] == "O"]:
        f.write("    if (%s->presencemask & NAS_%s_%s_PRESENT)\n" % (v_lower(k), v_upper(k), v_upper(ie["value"])))
        f.write("    {\n")
        if ie["length"] == "1" and ie["format"] == "TV":
            f.write("        %s->%s.type = (NAS_%s_%s_TYPE >> 4);\n\n" % (v_lower(k), v_lower(ie["value"]), v_upper(k), v_upper(ie["value"])))
        else:
            f.write("        size = nas_encode_optional_type(pkbuf, NAS_%s_%s_TYPE);\n" % (v_upper(k), v_upper(ie["value"])))
            f.write("        d_assert(size >= 0, return encoded, \"decode failed\");\n")
            f.write("        encoded += size;\n\n")
        f.write("        size = nas_encode_%s(pkbuf, &%s->%s);\n" % (v_lower(ie["type"]), v_lower(k), v_lower(ie["value"])))
        f.write("        d_assert(size >= 0, return encoded, \"decode failed\");\n")
        f.write("        encoded += size;\n")
        f.write("    }\n\n")

    f.write("""    return encoded;
}

""")


f.write("""status_t nas_gmm_encode(pkbuf_t **pkbuf, nas_message_t *message)
{
    status_t rv = CORE_ERROR;
    c_int32_t size = 0;
    c_int32_t encoded = 0;

    d_assert(message, return CORE_ERROR, "Null param");

    /* The Packet Buffer(pkbuf_t) for NAS message MUST make a HEADROOM. 
     * When calculating AES_CMAC, we need to use the headroom of the packet. */
    *pkbuf = pkbuf_alloc(NAS_HEADROOM, MAX_SDU_LEN);
    d_assert(*pkbuf, return -1, "Null Param");

    size = sizeof(nas_gmm_header_t);
    rv = pkbuf_header(*pkbuf, -size);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pkbuf_header error");

    memcpy((*pkbuf)->payload - size, &message->gmm.h, size);
    encoded += size;

    if (message->gmm.h.security_header_type >=
            NAS_SECURITY_HEADER_FOR_SERVICE_REQUEST_MESSAGE)
    {
        d_assert(pkbuf_header(*pkbuf, 1) == CORE_OK, 
                return CORE_ERROR, "pkbuf_header error");
        encoded -= 1;
        size = nas_encode_service_request(*pkbuf, message);
        d_assert(size >= 0, return CORE_ERROR, "decode error");
        encoded += size;

        goto out;
    }

    switch(message->gmm.h.message_type)
    {
""")

for (k, v) in sorted_msg_list:
    if "ies" not in msg_list[k]:
        continue;
    if float(msg_list[k]["type"]) < 192 and k.find("FROM UE") == -1 and k != "SERVICE REQUEST":
        f.write("        case NAS_%s:\n" % v_upper(k))
        if len(msg_list[k]["ies"]) != 0:
            f.write("            size = nas_encode_%s(*pkbuf, message);\n" % v_lower(k))
            f.write("            d_assert(size >= 0, return CORE_ERROR, \"decode error\");\n")
            f.write("            encoded += size;\n")
        f.write("            break;\n")

f.write("""        default:
            d_error("Unknown message type (0x%x) or not implemented", 
                    message->gmm.h.message_type);
            pkbuf_free((*pkbuf));
            return CORE_ERROR;
    }

out:
    rv = pkbuf_header(*pkbuf, encoded);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pkbuf_header error");

    (*pkbuf)->len = encoded;

    return CORE_OK;
}

""")

f.write("""status_t nas_gsm_encode(pkbuf_t **pkbuf, nas_message_t *message)
{
    status_t rv = CORE_ERROR;
    c_int32_t size = 0;
    c_int32_t encoded = 0;

    d_assert(message, return CORE_ERROR, "Null param");

    /* The Packet Buffer(pkbuf_t) for NAS message MUST make a HEADROOM. 
     * When calculating AES_CMAC, we need to use the headroom of the packet. */
    *pkbuf = pkbuf_alloc(NAS_HEADROOM, MAX_SDU_LEN);
    d_assert(*pkbuf, return -1, "Null Param");

    size = sizeof(nas_gsm_header_t);
    rv = pkbuf_header(*pkbuf, -size);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pkbuf_header error");

    memcpy((*pkbuf)->payload - size, &message->gsm.h, size);
    encoded += size;

    switch(message->gsm.h.message_type)
    {
""")

for (k, v) in sorted_msg_list:
    if "ies" not in msg_list[k]:
        continue;
    if float(msg_list[k]["type"]) >= 192:
        f.write("        case NAS_%s:\n" % v_upper(k))
        if len(msg_list[k]["ies"]) != 0:
            f.write("            size = nas_encode_%s(*pkbuf, message);\n" % v_lower(k))
            f.write("            d_assert(size >= 0, return CORE_ERROR, \"decode error\");\n")
            f.write("            encoded += size;\n")
        f.write("            break;\n")

f.write("""        default:
            d_error("Unknown message type (0x%x) or not implemented", 
                    message->gsm.h.message_type);
            pkbuf_free((*pkbuf));
            return CORE_ERROR;
    }

    rv = pkbuf_header(*pkbuf, encoded);
    d_assert(rv == CORE_OK, return CORE_ERROR, "pkbuf_header error");

    (*pkbuf)->len = encoded;

    return CORE_OK;
}

status_t nas_plain_encode(pkbuf_t **pkbuf, nas_message_t *message)
{
    d_assert(message, return CORE_ERROR, "Null param");

    d_assert(message->gmm.h.extended_protocol_discriminator ==
            message->gsm.h.extended_protocol_discriminator, 
            return CORE_ERROR, "check UNION for protocol");

    if (message->gmm.h.extended_protocol_discriminator == 
            NAS_EXTENDED_PROTOCOL_DISCRIMINATOR_GMM)
        return nas_emm_encode(pkbuf, message);
    else if (message->gmm.h.extended_protocol_discriminator == 
            NAS_EXTENDED_PROTOCOL_DISCRIMINATOR_GSM)
        return nas_esm_encode(pkbuf, message);

    d_assert(0, return CORE_ERROR, 
            "Invalid Protocol : %d", message->gmm.h.extended_protocol_discriminator);
}
""")

f.close()

