class ProtocolDataParser:
    __inputBuffer = ""
    __resultList = []

    def __init__(self, protocolStartSequence, protocolEndSequence):
        """Create a new parser object for parsing protocol data. A protocol is defined by a start and
    an end sequence. The content betweeen this sequences is the protocol data. The start and end 
    sequence strings must be given at construction and will be used for the parsing.
    """
        self.__startSeq = protocolStartSequence
        self.__endSeq = protocolEndSequence

    def parse(self, input):
        """Parse input string for protocol data. Protocol data is defined by a start
    and end sequence with the data between the sequences. The start and end sequence
    must be configured when an object of the parser is created. If the input contains 
    valid protocol data the function will return a list with the parsed data. This will
    result to a string list element for every piece of data between the protocol sequences.
    The protocol sequences will not be included in the output.
    """
        self.__parseImpl(input)

        returnList = self.__resultList.copy()
        del self.__resultList[:]  # delete list "after" results have been returned

        return returnList

    def __parseImpl(self, input):
        if input != "":  # received input is not empty
            # check if buffer contains data. means we found a start sequence in the past
            if self.__inputBuffer != "":
                self.__searchEndSequence(
                    input
                )  # then we need to find the corresponding end sequence
            else:
                self.__searchStartSequence(input)

    def __searchStartSequence(self, input):
        startSeqPos = input.find(self.__startSeq)
        if startSeqPos >= 0:  # check if start sequence was found
            # slice the input including start sequence and search for end sequence after
            dataStartPos = startSeqPos + len(self.__startSeq)
            self.__searchEndSequence(input[dataStartPos:])

    def __searchEndSequence(self, input):
        startSeqPos = input.find(self.__startSeq)
        endSeqPos = input.find(self.__endSeq)

        if startSeqPos >= 0:  # start sequence found while searching end sequence
            # this is not an error as long as we found an end sequence before this new start sequence
            # this could be a new command to process after the current command processing and it would be ok.
            # but if no end sequence found or the end sequence comes after the found start sequence then we must handle the error
            if endSeqPos < 0 or startSeqPos < endSeqPos:
                self.__inputBuffer = ""  # reset buffer and ignore the past data
                print(
                    "ProtocolDataParser parse error: Found a start sequence while searching for end sequence. Maybe data is missing. Parser will be reset."
                )
                self.__parseImpl(input)  # start parsing from new start sequence
                return  # do not continue to search the end sequence in error case

        if endSeqPos >= 0:  # valid end sequence found
            # write all data (excluding end sequence) to the buffer
            self.__inputBuffer += input[:endSeqPos]
            self.__processBufferData()  # process complete data in buffer
            restOfInput = input[endSeqPos + len(self.__endSeq) :]
            if len(restOfInput) > 0:  # check if there is still data to process
                self.__parseImpl(restOfInput)
        else:
            self.__inputBuffer += input  # store data and wait for further input

    def __processBufferData(self):
        """writes the contents of the input buffer to the result list 
    and deletes the buffer contents
    """
        if len(self.__inputBuffer) > 0:
            self.__resultList.append(self.__inputBuffer)
            self.__inputBuffer = ""
