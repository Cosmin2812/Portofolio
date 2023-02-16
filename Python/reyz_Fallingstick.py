from vpython import *
#GlowScript 3.2 VPython
g1=graph()
f1=gcurve()
L=1
ang0=150*pi/180
b1=sphere(pos=vector(0,0,0), radius=0.1, m=2)
b2=sphere(pos=L*vector(sin(ang0),cos(ang0),0), radius=0.1, m=2, color=color.cyan)
c=cylinder(pos=vector(0,0,0), axis=b2.pos-b1.pos, radius=0.02)
I=b2.m*mag(c.axis)**2
t=0
dt=0.001
a=0
omega=vector(0,0,0)
ang=ang0
g=vector(0,-9.8,0)
while(t<100):
    rate(1000)
    c.axis=b2.pos-b1.pos
    M=b2.m*cross(c.axis,g)
    a=M/I
    omega=omega+a*dt
    ang=ang-omega.z*dt
    b2.pos=L*vector(sin(ang),cos(ang),0)
    E=I*mag(omega)**2/2
    f1.plot(t,ang)
    t=t+dt