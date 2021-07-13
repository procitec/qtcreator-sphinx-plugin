import sys, os

sys.path.append(os.path.dirname(__file__))

import protocol_data_parser
import sys
import json
import io
import logging
import json
# create standard python (console) logger

logging.basicConfig()
logger = logging.getLogger()
logger.setLevel(logging.WARNING)
# Activate this line  to get debug output of the initialization process
#logger.setLevel( logging.DEBUG )
# set True to enable file logging (only for use during developing the parser)
enable_file_logging = True

try:
    import locale  # module missing in Jython

    locale.setlocale(locale.LC_ALL, "")
except locale.Error:
    pass

from docutils.core import publish_string
from docutils.utils.error_reporting import ErrorString


class Preview:
    def __init__(self, protocolStartSequence, protocolEndSequence):
        if protocolStartSequence and protocolEndSequence:
            self.startSeq = protocolStartSequence
            self.endSeq = protocolEndSequence
            self.parser = protocol_data_parser.ProtocolDataParser(
                protocolStartSequence, protocolEndSequence
            )

            # self.overrides = {'halt': 5, 'traceback': False, 'exit_status': 5, 'report_level': 5}
            self.overrides = {"report_level": 5, "output_encoding": "utf-8","embed_images": True }
        else:
            raise RuntimeError(
                "ERROR: Invalid protocol start and end sequences. They cannot be empty."
            )

    def process(self, inputStr):
        if inputStr:
            result = self.parser.parse(inputStr)
            if result:
                logger.debug("process(): parser result: %s", result)
                self.__processCommands(result)

    def __debugOutput(self, enable):
        if enable:
            logger.setLevel(logging.DEBUG)
        else:
            logger.setLevel(logging.WARNING)

        print(f"{self.endSeq}", flush=True)

    def __processCommands(self, commands):
        content = "\n".join(commands)
        j_content = json.loads(content)

        try:
            output = None
            errors = None
            output = publish_string(
                source=j_content["source"],source_path=j_content["source_path"], writer_name="html5", settings_overrides=self.overrides
            ).decode("utf-8")


            error_stream = io.StringIO()#write_through=True
            overrides_error = {"report_level": 2, "output_encoding": "utf-8", "warning_stream": error_stream }

            error_out=publish_string(
                source=j_content["source"],source_path=j_content["source_path"], writer_name="html5", settings_overrides=overrides_error
            ).decode("utf-8")

            errors = error_stream.getvalue()
            error_stream.close()
        except Exception as error:
            output = ErrorString(error)
            logger.error(f"{output}")
            print(f"{self.endSeq}", flush=True)

        #logger.debug(f"{output}")
        if 0 < len(errors):
            print(f"{errors}{self.endSeq}", file=sys.stderr, flush=True)
        print(f"{output}")
        print(f"{self.endSeq}", flush=True)


###########################
# MAIN processing routine #
###########################

if len(sys.argv) < 3:
    raise RuntimeError(
        "ERROR: Missing arguments.\nUsage: rest_to_html.py protocol_start_sequence protocol_end_sequence"
    )
else:

    if len(sys.argv) >= 4 and enable_file_logging:
        logfile_name = sys.argv[3]
        fh = logging.FileHandler(logfile_name)
        logger.addHandler(fh)

    logger.debug("MAIN() Completion script: %s", sys.argv[0])
    logger.debug("MAIN() protocol_start_sequence: %s", sys.argv[1])
    logger.debug("MAIN() protocol_end_sequence: %s", sys.argv[2])
    protocolStartSequence = sys.argv[1]
    protocolEndSequence = sys.argv[2]
    preview = Preview(protocolStartSequence, protocolEndSequence)
    input_stream = io.TextIOWrapper(
        sys.stdin.buffer, encoding="utf-8"
    )  # set stdin encoding to utf-8

    # read continously from stdin
    for line in input_stream:
        logger.debug("MAIN() new stdin: %s", line)
        preview.process(line)
