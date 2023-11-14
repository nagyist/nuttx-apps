#!/usr/bin/env python3
############################################################################
# apps/testing/cmocka/tools/cmocka_report.py
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

import json
from enum import Enum

import typer
import xmltodict
from bs4 import BeautifulSoup


class ConvertType(str, Enum):
    XML2JSON = "xml2json"


class CmockaReport:
    def __init__(self, xml, out):
        self.xml = xml
        self.out = out

    def open_xml(self):
        """Parse the XML file and convert it into a dictionary"""
        try:
            with open(self.xml, "r") as f:
                content = f.read()
                soup = BeautifulSoup(content, "xml")
                xml_dict = xmltodict.parse(str(soup))
                return xml_dict
        except FileNotFoundError:
            print("No such file or directory: {0}".format(self.xml))
        except Exception:
            print("Failed to parse XML file")

    def xml2json(self):
        """Convert XML dictionary into a JSON string"""
        xml_dict = self.open_xml()
        if xml_dict is None:
            return

        json_data = json.dumps(xml_dict, indent=4)
        if self.out:
            try:
                f = open(self.out, "w")
                f.write(json_data)
                f.close()
                print("Job Done")
            except FileNotFoundError:
                print("No such file or directory: {0}".format(self.out))
            except Exception:
                print("Failed to write json file")
        else:
            print(json_data)


app = typer.Typer()


@app.command()
def main(
    operate: ConvertType = typer.Option(
        default=ConvertType.XML2JSON, help="operation type"
    ),
    xml: str = typer.Option(default=None, help="where is the xml file"),
    out: str = typer.Option(default=None, help="write to output instead of stdout"),
):
    """
    :param operate: operation type
    :param xml: where where xml file
    :param out: write to output instead of stdout
    :return:
    """
    rpt = CmockaReport(xml, out)
    if operate == ConvertType.XML2JSON:
        rpt.xml2json()


if __name__ == "__main__":
    app()
