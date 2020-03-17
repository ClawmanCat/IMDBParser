import os
import sys


# CMake will change the CWD to its build directory, we don't want that for running Conan
cwd = os.path.dirname(os.path.realpath(__file__))
oldcwd = os.getcwd()

os.chdir(cwd)


# Read command line args
require_pdb = False

for i, arg in enumerate(sys.argv):
	if (arg == '--require-pdb'):
		require_pdb = True


# Create folders if they don't yet exist
if not os.path.exists(cwd + '/Debug'):
	os.makedirs(cwd + '/Debug')

if not os.path.exists(cwd + '/Release'):
	os.makedirs(cwd + '/Release')


print("Running conan for debug configuration...")

if (require_pdb):
	os.system('conan install . --profile ./debug.conanprofile --install-folder "' + cwd + '/Debug/" --build')
else:
	os.system('conan install . --profile ./debug.conanprofile --install-folder "' + cwd + '/Debug/" --build missing')
	

print("Running conan for release configuration...")

os.system('conan install . --profile ./release.conanprofile --install-folder "' + cwd + '/Release/" --build missing')


os.chdir(oldcwd)