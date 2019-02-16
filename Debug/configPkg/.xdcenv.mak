#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /Users/hunteradams/ti/simplelink_cc13x0_sdk_2_30_00_20/source;/Users/hunteradams/ti/simplelink_cc13x0_sdk_2_30_00_20/kernel/tirtos/packages
override XDCROOT = /Applications/ti/xdctools_3_50_02_20_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /Users/hunteradams/ti/simplelink_cc13x0_sdk_2_30_00_20/source;/Users/hunteradams/ti/simplelink_cc13x0_sdk_2_30_00_20/kernel/tirtos/packages;/Applications/ti/xdctools_3_50_02_20_core/packages;..
HOSTOS = MacOS
endif
