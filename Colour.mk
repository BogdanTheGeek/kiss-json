
# define standard colors
ifneq (,$(findstring 256color,${TERM}))
	ERROR    := $(shell tput -Txterm setaf 1)
	SUCCESS := $(shell tput -Txterm setaf 2)
	WARNING := $(shell tput -Txterm setaf 3)
	JAZZ    := $(shell tput -Txterm setaf 4)
	RESET   := $(shell tput -Txterm sgr0)
else
	ERROR    := ""
	SUCCESS := ""
	WARNING := ""
	JAZZ    := ""
endif
