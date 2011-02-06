VERSION='0.1'
APPNAME='trivial'

top = '.'
build = 'build'

def options(opt):
    opt.load('compiler_cxx')
    opt.add_option('--profile', action='store', default=True,
                   help="Enable profiling")

def configure(conf):
    flags = "-Wall -Wno-sign-compare -std=c++0x".split()
    optimize = "-O2 -march=native".split()
    debug = "-g3"

    conf.load('compiler_cxx')

    conf.env.append_value('CXXFLAGS', flags)
    conf.env.append_value('CXXFLAGS', optimize)
    conf.env.append_value('CXXFLAGS', debug)

    conf.check_cxx(lib='glfw', uselib_store='OPENGL')
    conf.check_cxx(lib='GL', uselib_store='OPENGL')
    conf.check_cxx(lib='GLU', uselib_store='OPENGL')

    if conf.options.profile:
        conf.check_cxx(lib='profiler', uselib_store='PROFILER')

def build(ctx):
    ctx.objects(source=ctx.path.ant_glob("*.cpp"),
                target='objects')

    sources = ctx.path.ant_glob("*.cpp")
    ctx.recurse("tests")

