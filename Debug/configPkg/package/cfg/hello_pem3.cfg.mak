# invoke SourceDir generated makefile for hello.pem3
hello.pem3: .libraries,hello.pem3
.libraries,hello.pem3: package/cfg/hello_pem3.xdl
	$(MAKE) -f /Users/hunteradams/Documents/MSP430/workspace/Monarch_Software/src/makefile.libs

clean::
	$(MAKE) -f /Users/hunteradams/Documents/MSP430/workspace/Monarch_Software/src/makefile.libs clean

