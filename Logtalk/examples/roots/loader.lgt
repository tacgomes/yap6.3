
:- initialization((
	logtalk_load(
		[library(events_loader), library(types_loader), library(hierarchies_loader)],
		[reload(skip)]),		% allow for static binding
	logtalk_load(
		[initialization, classes, prototypes, nil],
		[unknown(silent)]))).
