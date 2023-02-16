from vpython import *
#GlowScript 3.2 VPython
tgraph=graph(xtitle="Time [s]", ytitle="Angular Momentum [kg*m^2/s", fast=True)
fA=gcurve(color=color.blue, label="Mass A")
fB=gcurve(color=color.red, label="Mass B")
fC=gcurve(color=color.green, label="Mass C")
ftot=gcurve(label="Total")

#dimensions
#s1 is the length
s1=0.05
#s2 is the width of the T
s2=0.07
#m1 is the mass on the end
m1=0.3
#m2 are the masses on the handle
m2=0.4
#omega is the angular velocity
omega=vector(2,0.01,0) 
k=2000
massA=sphere(pos=vector(0,s2/2,0), radius=0.005, color=color.yellow, make_trail=True, retain=20)
massA.m=m2
massB=sphere(pos=vector(0,-s2/2,0), radius=0.005, color=color.red, make_trail=True, retain=20)
massB.m=m2
massC=sphere(pos=vector(s1,0,0), radius=0.005,color=color.cyan, make_trail=True, retain=20)
massC.m=m1
com=(massA.m*massA.pos+massB.m*massB.pos+massC.m*massC.pos)/(massA.m+massB.m+massC.m)
#r123 are the position vectors relative to com
r1=massA.pos-com
r2=massB.pos-com
r3=massC.pos-com
#velocities are calculated so that the axis of rotation goes through com
#we consider that 
massA.v=cross(omega,r1)
massA.p=massA.m*massA.v
massB.v=cross(omega,r2)
massB.p=massB.m*massB.v
massC.v=cross(omega,r3)
massC.p=massC.m*massC.v
rAB=-(massA.pos-massB.pos)
rBC=-(massB.pos-massC.pos)
rCA=-(massC.pos-massA.pos)
springAB=cylinder(pos=massA.pos, axis=rAB, radius=.001)
springBC=cylinder(pos=massB.pos, axis=rBC, radius=.001)
springCA=cylinder(pos=massC.pos, axis=rCA, radius=.001)
LAB=mag(rAB)
LBC=mag(rBC)
LCA=mag(rCA)
t=0
dt=0.001
while(t<50):
    rate(1000)
    rAB=-(massA.pos-massB.pos)
    rBC=-(massB.pos-massC.pos)
    rCA=-(massC.pos-massA.pos)
    F1=-k*(mag(rAB)-LAB)*norm(rAB)
    F2=-k*(mag(rBC)-LBC)*norm(rBC)
    F3=-k*(mag(rCA)-LCA)*norm(rCA)
    massA.p=massA.p+(F3-F1)*dt
    massB.p=massB.p+(F1-F2)*dt
    massC.p=massC.p+(F2-F3)*dt
    massA.pos=massA.pos+(massA.p/massA.m)*dt
    massB.pos=massB.pos+(massB.p/massB.m)*dt
    massC.pos=massC.pos+(massC.p/massC.m)*dt
    springAB.pos=massA.pos
    springAB.axis=rAB
    springBC.pos=massB.pos
    springBC.axis=rBC
    springCA.pos=massC.pos
    springCA.axis=rCA
    r1=massA.pos-com
    r2=massB.pos-com
    r3=massC.pos-com
    LA=cross(r1,massA.p)
    LB=cross(r2,massB.p)
    LC=cross(r3,massC.p)
    fA.plot(t,LA.z)
    fB.plot(t,LB.z)
    fC.plot(t,LC.z)
    ftot.plot(t,LA.z+LB.z+LC.z)
    t=t+dt
    
    





