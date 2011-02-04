VERSION=''
APPNAME='trivial'

from os.path import splitext, basename

def set_options(opt):
    opt.tool_options('compiler_cxx')
    opt.tool_options('boost')

def configure(conf):
    for i in """-Wall -O2 -Wno-sign-compare -g3 -march=native -std=c++0x""".split():
        conf.env.append_value('CXXFLAGS', i)

    conf.env.append_value('LINKFLAGS', '-pg')
    conf.env.append_value('CXXFLAGS', '-pg')

    conf.check_tool('compiler_cxx')
    conf.check_tool('boost')

    conf.check_cxx(lib='glfw', uselib_store='GLFW')
    conf.check_cxx(lib='GL', uselib_store='GLFW')
    conf.check_cxx(lib='GLU', uselib_store='GLFW')

#    conf.check_boost(lib='random', min_version='1.42',
#            uselib_store='BOOST')

def build(bld):
    sources = bld.glob("*.cpp")
    for i in bld.glob("tests/*.cpp"):
        bld(features = 'cxx cprogram',
            source = [i] + sources,
            target="trivial-%s" % (splitext(basename(i))[0]),
            uselib="GLFW",
            includes=".. .")

