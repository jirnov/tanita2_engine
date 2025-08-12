# coding: cp1251
import sys, os.path

def check_version( versionstr, versionfile ):
    '''
    Возвращает True, если строка с версией совпадает с содержимым файла.
    '''
    if not os.path.exists(versionfile):
        return False
    return versionstr == file(versionfile, 'rt').read()


def update_build_revision( template, version, destination ):
    '''
    Записывает в файл destination содержимое шаблона с заменой
    номера версии из файла version. Запись производится только если
    содержимое изменилось. Возвращает True, если файл был обновлен.
    '''
    tmpl = file(template, 'rt').read()
    version = file(version, 'rt').read()
    
    dst = ""
    if os.path.exists(destination):
        dst = file(destination, 'rt').read()
    
    content = tmpl.replace('L"not versioned"', 'L"%s"' % version)
    hasModifications = (-1 != version.find("with local modifications"))
    if not hasModifications:
        content = content.replace("projectIsNotVersioned_type::smValue = true;",
                                  "projectIsNotVersioned_type::smValue = false;")
    
    if content != dst:
        file(destination, 'wt').write(content)
        return True
    return False
