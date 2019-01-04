#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/source;/Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/kernel/tirtos/packages;/Applications/ti/ccsv8/ccs_base
override XDCROOT = /Applications/ti/xdctools_3_50_02_20_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/source;/Applications/ti/simplelink_cc13x0_sdk_1_40_00_10/kernel/tirtos/packages;/Applications/ti/ccsv8/ccs_base;/Applications/ti/xdctools_3_50_02_20_core/packages;..
HOSTOS = MacOS
endif
