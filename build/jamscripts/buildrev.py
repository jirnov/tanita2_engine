# coding: cp1251
import sys, os, os.path
from subprocess import Popen, PIPE

def exit( message ):
    '''
    ����� ��������� �� ������ � stderr � ���������� ������.
    '''
    print >> sys.stderr, "Error:", message
    sys.exit(1)

def hg( *args ):
    '''
    �������� mercurial � ���������� stdout, ���� �� ���� ������.
    '''
    try:
        hgprocess = Popen(["hg"] + list(args), stdout=PIPE)
    except WindowsError:
        exit("hg not in PATH")
    
    out, err = hgprocess.communicate()
    if hgprocess.returncode != 0:
        exit("mercurial error")
    return out

def get_status( directory='.' ):
    '''
    ���������� ������, ��������������� ������� ��������� ��������� � �����������.
    ��������� ��������:
        ""
        " with local modifications"
    '''
    currentDir = os.getcwd()
    os.chdir(directory)
    
    retVal = ""
    if hg("st", '-mard'):
        retVal = " with local modifications"
    os.chdir(currentDir)
    return retVal
    
def get_patches( directory='.' ):
    '''
    ���������� ������ �� ������� ����������� ������.
    '''
    currentDir = os.getcwd()
    os.chdir(directory)
    patches = hg("qap").split()
    if patches:
        return "'" + "', '".join(patches) + "'"
    else:
        return ""

def get_tip_revision( directory='.' ):
    '''
    ���������� ������ ����� ������� tip. ���� ���������� mercurial queues,
    ���������� �� tip, � qparent.
    '''
    currentDir = os.getcwd()
    os.chdir(directory)
    
    if os.path.exists('.hg\\patches\\status') and os.path.getsize('.hg\\patches\\status') != 0:
        revision = hg("log", '-r', 'qparent', '--template', '{node}')
    else:
        revision = hg("tip", '--template', '{node}')
    os.chdir(currentDir)
    return revision[:12]

def get_version():
    '''
    ���������� ����� ������� � ���������� � ������.
    '''
    if not os.path.isdir('.hg'):
        return "not versioned"
    version = "rev. %s%s" % (get_tip_revision(), get_status())
    if os.path.isdir('.hg\\patches'):
        patches = get_patches()
        if patches:
            version += " with patches %s (rev. %s%s)" % (patches,
                                                         get_tip_revision('.hg\\patches'),
                                                         get_status('.hg\\patches'))
    return version
