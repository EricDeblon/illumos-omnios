#
# CDDL HEADER START
#
# The contents of this file are subject to the terms of the
# Common Development and Distribution License (the "License").
# You may not use this file except in compliance with the License.
#
# You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
# or http://www.opensolaris.org/os/licensing.
# See the License for the specific language governing permissions
# and limitations under the License.
#
# When distributing Covered Code, include this CDDL HEADER in each
# file and include the License file at usr/src/OPENSOLARIS.LICENSE.
# If applicable, add the following below this CDDL HEADER, with the
# fields enclosed by brackets "[]" replaced with your own identifying
# information: Portions Copyright [yyyy] [name of copyright owner]
#
# CDDL HEADER END
#

#
# Copyright 2016 Joyent, Inc.
# Copyright 2019 OmniOS Community Edition (OmniOSce) Association.
#

import re, sys

spellMsg = 'contains "{}", a common misspelling of "{}"'
altMsg = 'contains "{}"; please use "{}" instead for consistency with other documentation'
caseMsg = 'contains "{}"; please use "{}" instead'

misspellings = {
	'absense': 'absence',
	'accessable': 'accessible',
	'accomodate': 'accommodate',
	'accomodation': 'accommodation',
	'accross': 'across',
	'acheive': 'achieve',
	'addional': 'additional',
	'addres': 'address',
	'admininistrative': 'administrative',
	'adminstered': 'administered',
	'adminstrate': 'administrate',
	'adminstration': 'administration',
	'adminstrative': 'administrative',
	'adminstrator': 'administrator',
	'admissability': 'admissibility',
	'adress': 'address',
	'adressable': 'addressable',
	'adressed': 'addressed',
	'adressing': 'addressing, dressing',
	'aginst': 'against',
	'agression': 'aggression',
	'agressive': 'aggressive',
	'alot': 'a lot, allot',
	'and and': 'and',
	'apparantly': 'apparently',
	'appearence': 'appearance',
	'arguement': 'argument',
	'assasination': 'assassination',
	'auxilliary': 'auxiliary',
	'basicly': 'basically',
	'begining': 'beginning',
	'belive': 'believe',
	'beteen': 'between',
	'betwen': 'between',
	'beween': 'between',
	'bewteen': 'between',
	'bizzare': 'bizarre',
	'buisness': 'business',
	'calender': 'calendar',
	'cemetary': 'cemetery',
	'chauffer': 'chauffeur',
	'collegue': 'colleague',
	'comming': 'coming',
	'commited': 'committed',
	'commitee': 'committee',
	'commiting': 'committing',
	'comparision': 'comparison',
	'comparisions': 'comparisons',
	'compatability': 'compatibility',
	'compatable': 'compatible',
	'compatablity': 'compatibility',
	'compatiable': 'compatible',
	'compatiblity': 'compatibility',
	'completly': 'completely',
	'concious': 'conscious',
	'condidtion': 'condition',
	'conected': 'connected',
	'conjuction': 'conjunction',
	'continous': 'continuous',
	'curiousity': 'curiosity',
	'deamon': 'daemon',
	'definately': 'definitely',
	'desireable': 'desirable',
	'diffrent': 'different',
	'dilemna': 'dilemma',
	'dissapear': 'disappear',
	'dissapoint': 'disappoint',
	'ecstacy': 'ecstasy',
	'embarass': 'embarrass',
	'enviroment': 'environment',
	'exept': 'except',
	'existance': 'existence',
	'familar': 'familiar',
	'finaly': 'finally',
	'folowing': 'following',
	'foriegn': 'foreign',
	'forseeable': 'foreseeable',
	'fourty': 'forty',
	'foward': 'forward',
	'freind': 'friend',
	'futher': 'further',
	'gaurd': 'guard',
	'glamourous': 'glamorous',
	'goverment': 'government',
	'happend': 'happened',
	'harrassment': 'harassment',
	'hierachical': 'hierarchical',
	'hierachies': 'hierarchies',
	'hierachy': 'hierarchy',
	'hierarcical': 'hierarchical',
	'hierarcy': 'hierarchy',
	'honourary': 'honorary',
	'humourous': 'humorous',
	'idiosyncracy': 'idiosyncrasy',
	'immediatly': 'immediately',
	'inaccessable': 'inaccessible',
	'inbetween': 'between',
	'incidently': 'incidentally',
	'independant': 'independent',
	'infomation': 'information',
	'interupt': 'interrupt',
	'intial': 'initial',
	'intially': 'initially',
	'irresistable': 'irresistible',
	'jist': 'gist',
	'knowlege': 'knowledge',
	'lenght': 'length',
	'liase': 'liaise',
	'liason': 'liaison',
	'libary': 'library',
	'maching': 'machine, marching, matching',
	'millenia': 'millennia',
	'millenium': 'millennium',
	'neccessary': 'necessary',
	'negotation': 'negotiation',
	'nontheless': 'nonetheless',
	'noticable': 'noticeable',
	'occassion': 'occasion',
	'occassional': 'occasional',
	'occassionally': 'occasionally',
	'occurance': 'occurrence',
	'occured': 'occurred',
	'occurence': 'occurrence',
	'occuring': 'occurring',
	'ommision': 'omission',
	'orginal': 'original',
	'orginally': 'originally',
	'ouput': 'output',
	'overriden': 'overridden',
	'particuliar': 'particular',
	'pavillion': 'pavilion',
	'peice': 'piece',
	'persistant': 'persistent',
	'politican': 'politician',
	'posession': 'possession',
	'possiblity': 'possibility',
	'preceed': 'precede',
	'preceeded': 'preceded',
	'preceeding': 'preceding',
	'preceeds': 'precedes',
	'prefered': 'preferred',
	'prefering': 'preferring',
	'presense': 'presence',
	'proces': 'process',
	'propoganda': 'propaganda',
	'psuedo': 'pseudo',
	'publically': 'publicly',
	'realy': 'really',
	'reciept': 'receipt',
	'recieve': 'receive',
	'recieved': 'received',
	'reciever': 'receiver',
	'recievers': 'receivers',
	'recieves': 'receives',
	'recieving': 'receiving',
	'recomend': 'recommend',
	'recomended': 'recommended',
	'recomending': 'recommending',
	'recomends': 'recommends',
	'recurse': 'recur',
	'recurses': 'recurs',
	'recursing': 'recurring',
	'refered': 'referred',
	'refering': 'referring',
	'religous': 'religious',
	'rember': 'remember',
	'remeber': 'remember',
	'repetion': 'repetition',
	'reponsible': 'responsible',
	'resistence': 'resistance',
	'retreive': 'retrieve',
	'seige': 'siege',
	'sence': 'since',
	'seperate': 'separate',
	'seperated': 'separated',
	'seperately': 'separately',
	'seperates': 'separates',
	'similiar': 'similar',
	'somwhere': 'somewhere',
	'sould': 'could, should, sold, soul',
	'sturcture': 'structure',
	'succesful': 'successful',
	'succesfully': 'successfully',
	'successfull': 'successful',
	'sucessful': 'successful',
	'supercede': 'supersede',
	'supress': 'suppress',
	'supressed': 'suppressed',
	'suprise': 'surprise',
	'suprisingly': 'surprisingly',
	'sytem': 'system',
	'tendancy': 'tendency',
	'the the': 'the',
	'the these': 'these',
	'therefor': 'therefore',
	'threshhold': 'threshold',
	'tolerence': 'tolerance',
	'tommorow': 'tomorrow',
	'tommorrow': 'tomorrow',
	'tounge': 'tongue',
	'tranformed': 'transformed',
	'transfered': 'transferred',
	'truely': 'truly',
	'trustworthyness': 'trustworthiness',
	'uncommited': 'uncommitted',
	'unforseen': 'unforeseen',
	'unfortunatly': 'unfortunately',
	'unsuccessfull': 'unsuccessful',
	'untill': 'until',
	'upto': 'up to',
	'whereever': 'wherever',
	'wich': 'which',
	'wierd': 'weird',
	'wtih': 'with',
}

alternates = {
	'judgement': 'judgment',
	'parseable': 'parsable',
	'sub-command': 'subcommand',
	'sub-commands': 'subcommands',
	'writeable': 'writable'
}

case = {
	'Illumos': 'illumos'
}

misspellingREs = []
alternateREs = []
caseREs = []

for misspelling, correct in misspellings.items():
	regex = re.compile(r'\b%s\b' % (misspelling), re.IGNORECASE)
	entry = (regex, misspelling, correct)
	misspellingREs.append(entry)

for alternate, correct in alternates.items():
	regex = re.compile(r'\b%s\b' % (alternate), re.IGNORECASE)
	entry = (regex, alternate, correct)
	alternateREs.append(entry)

for alternate, correct in case.items():
	regex = re.compile(r'\b%s\b' % (alternate))
	entry = (regex, alternate, correct)
	caseREs.append(entry)

def spellcheck_line(line):
	errs = []
	for entry in misspellingREs:
		if entry[0].search(line):
			errs.append(spellMsg.format(entry[1], entry[2]))
	for entry in alternateREs:
		if entry[0].search(line):
			errs.append(altMsg.format(entry[1], entry[2]))
	for entry in caseREs:
		if entry[0].search(line):
			errs.append(caseMsg.format(entry[1], entry[2]))
	return errs

def spellcheck(fh, filename=None, output=sys.stderr, **opts):
	lineno = 1
	ret = 0

	if not filename:
		filename = fh.name

	fh.seek(0)
	for line in fh:
		line = line.decode(errors='replace')
		for err in spellcheck_line(line):
			output.write('{}: Line {} {}\n'.format(
			    filename, lineno, err))
			ret = 1
		lineno += 1

	return ret
