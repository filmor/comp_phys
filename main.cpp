
int main ()
{
    world w;

    world_gl_visitor gl;

    for (;;)
    {
        w.update();

        if (i % 100)
            w.visit(gl);
        w.visit(statistics);
    }
    statistics.print();
}
