from vpython import *
#GlowScript 3.2 VPython
g=graph(width=600, height=700)
phase=gcurve(color=color.black)
m1=box(pos=vector(0,0,0), size=vector(0.5,0.5,0.5), color=color.yellow, mass=1000, p=vector(0,0,0))
m2=box(pos=vector(2,0,0), size=vector(0.5,0.5,0.5), color=color.cyan, mass=1, p=vector(0,0,0))
wall=box(pos=vector(6,0,0), size=vector(0.1,1,1))
m1.p=vector(100000,0,0)
k=100000000
t=0
dt=0.0001
F1=vector(0,0,0)
F2=vector(0,0,0)
col1=0
col2=0
collisions=0
while(t<1.5):
    rate(10000)
    r1=m2.pos-m1.pos
    F1=vector(0,0,0)
    F2=vector(0,0,0)
    F3=vector(0,0,0)
    if(mag(r1)<0.5):
        F1=k*(mag(r1)-0.5)*norm(r1)
        F2=-F1
        col1=1
    else:
        if(col1==1):
            collisions=collisions+1
            col1=0
    r2=wall.pos-m2.pos
    if(mag(r2)<0.3):
        F3=k*(mag(r2)-0.3)*norm(r2)
        col2=1
    else:
        if(col2==1):
            collisions=collisions+1
            col2=0  
    m1.p=m1.p+F1*dt
    m1.pos=m1.pos+m1.p/m1.mass*dt
    m2.p=m2.p+(F2+F3)*dt
    m2.pos=m2.pos+m2.p/m2.mass*dt
    phase.plot(sqrt(m1.mass)*(m1.p.x/m1.mass),sqrt(m2.mass)*(m2.p.x/m2.mass))
    t=t+dt
    print(collisions)
    
        