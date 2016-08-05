import sys
import zipfile

if __name__ == '__main__':
    zf = zipfile.PyZipFile('zipfile_pyzipfile.zip', mode='w')
    try:
        zf.debug = 3
        print( 'Adding python files' )
        zf.writepy('.')
    finally:
        zf.close()
    for name in zf.namelist():
        print(name)

    print
    sys.path.insert(0, 'zipfile_pyzipfile.zip')
    #import zipfile_pyzipfile
    #print 'Imported from:', zipfile_pyzipfile.__file__