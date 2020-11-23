import sys, os

sys.path.append(os.path.dirname(__file__))

import protocol_data_parser
import sys
import json
import io
import logging

# create standard python (console) logger

logging.basicConfig()
logger = logging.getLogger()
logger.setLevel(logging.WARNING)
# Activate this line  to get debug output of the initialization process
logger.setLevel( logging.DEBUG )

# activate this line if you want also log to file. Adapt the path to your needs.
fh = logging.FileHandler('log.txt')
logger.addHandler(fh)


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
      self.startSeq=protocolStartSequence
      self.endSeq=protocolEndSequence
      self.parser=protocol_data_parser.ProtocolDataParser(protocolStartSequence, protocolEndSequence)
      self.htmlBuffer = []
      #self.overrides = {'halt': 5, 'traceback': False, 'exit_status': 5, 'report_level': 5}
      self.overrides = {'report_level': 5, 'output_encoding': 'utf-8'}
      #overrides['input_encoding'] = 'unicode'
    else:
      raise RuntimeError("ERROR: Invalid protocol start and end sequences. They cannot be empty.")

  def process(self, inputStr):
    if inputStr:
      result = self.parser.parse(inputStr)
      if result:
        logger.debug("process(): parser result: %s", result)
        self.__processCommands(result)

  def __debugOutput(self, enable):
    if enable:
      logger.setLevel( logging.DEBUG )
    else:
      logger.setLevel( logging.WARNING )

    print(f"{self.endSeq}", flush=True);
    
  def __processCommands(self, commands):
    content = "\n".join(commands)
    try:
        output = publish_string(source = content,
            writer_name="html5", settings_overrides=self.overrides
        ).decode('utf-8')
    except Exception as error:
        output=ErrorString(error)

    logger.debug(f"{output}")
    print(f"{output}")
    print(f"{self.endSeq}", flush=True);

###########################
# MAIN processing routine #
###########################

if len(sys.argv) < 3:
  raise RuntimeError("ERROR: Missing arguments.\nUsage: rest_to_html.py protocol_start_sequence protocol_end_sequence")
else:
  logger.debug("MAIN() Completion script: %s", sys.argv[0])
  logger.debug("MAIN() protocol_start_sequence: %s", sys.argv[1] )
  logger.debug("MAIN() protocol_end_sequence: %s", sys.argv[2])
  protocolStartSequence = sys.argv[1]
  protocolEndSequence = sys.argv[2]
  preview = Preview(protocolStartSequence, protocolEndSequence)
  input_stream = io.TextIOWrapper(sys.stdin.buffer, encoding='utf-8') #set stdin encoding to utf-8

  # read continously from stdin
  for line in input_stream:
    logger.debug("MAIN() new stdin: %s", line)
    preview.process(line)

