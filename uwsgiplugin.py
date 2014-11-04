import os

NAME = 'moarvm'

MOARVM_PATH='/Users/roberto/MoarVM/install'

CFLAGS = ['-Wno-error']
for item in os.listdir(MOARVM_PATH+'/include'):
    if item in ('msinttypes',):
        continue
    CFLAGS.append('-I%s/include/%s' % (MOARVM_PATH, item))

LDFLAGS=['-L%s/lib' % MOARVM_PATH]
LIBS=['-lmoar']
GCC_LIST=['moarvm']
