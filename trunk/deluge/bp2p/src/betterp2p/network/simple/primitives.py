

class Command(object):
    """
    A command is received or sent by a peer. It has a textual format
    which always conform to the following pattern:
    name arg1 arg2 ...
    """
    
    # should be overridden by subclasses
    NAME = None
    ARG_TYPES = []
    
    class ParseError(ValueError):
        def __init__(self, command_string, error_msg):
            super(Command.ParseError, self).__init__(error_msg)
            self.command_string = command_string
        def __str__(self):
            return "'%s': %s" % (self.command_string, 
                                 super(Command.ParseError, self).__str__())
    
    def __init__(self, **cmd_args):
        self.__dict__.update(cmd_args)

    def __repr__(self):
        arg_vals = [(arg_name, self.__dict__.get(arg_name, None)) 
                    for (arg_name, arg_type) in self.ARG_TYPES]
        fmtd = ["%s=%r" % kv for kv in arg_vals]
        return "%s[%s]" % (self.NAME, ",".join(fmtd))

    def __str__(self):
        arg_vals = [self.__dict__.get(arg_name, None) 
                    for (arg_name, arg_type) in self.ARG_TYPES]
        fmtd = [str(v) for v in arg_vals]
        return " ".join([self.NAME] + fmtd)

    @classmethod
    def parse(cls, line):
        if len(line) == 0:
            raise Command.ParseError(line, "empty command")
        # Split line to name and args
        splt = line.split()
        name = splt[0]
        args = splt[1:]
        # Find a subclass which matches
        for command in [cls] + cls.__subclasses__():
            if name == command.NAME:
                fmtd_args = cls._format_args(line, command.ARG_TYPES, args)
                return command(**fmtd_args)
        else:
            raise Command.ParseError(line, "unrecognized command '%s'" % name)
    
    @classmethod
    def _format_args(cls, line, arg_types, args):
        if len(args) != len(arg_types):
            raise Command.ParseError(line, "expected %i arguments, got %i" %\
                                     (len(arg_types), len(args)))
        # Format according to arg_types
        fmtd_args = {}
        for i in xrange(len(args)):
            arg_name, arg_type = arg_types[i]
            try:
                fmtd_args[arg_name] = arg_type(args[i])
            except ValueError:
                msg = "argument #%i should be %s, got '%s'" % \
                        (i+1, arg_type.__name__, args[i])
                raise Command.ParseError(line, msg)
        
        return fmtd_args


class HelloCommand(Command):
    NAME="HELO"
    ARG_TYPES=[('alias', str)]

class InterestedCommand(Command):
    NAME="INTERESTED"
    ARG_TYPES=[('file_id', str), ('transit_port', int)]

class ChunkCommand(Command):
    NAME="CHUNK"
    ARG_TYPES=[('file_id', str), ('chunk_id', long), ('header_size', int), ('size', int)]

class OkCommand(Command):
    NAME="OK"

class ByeCommand(Command):
    NAME="BYE"

