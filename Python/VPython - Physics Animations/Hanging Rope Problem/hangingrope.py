from vpython import *
#GlowScript 3.2 VPython
N=20
K=1000
M=100
L=5
g=vector(0,-9.8,0)
air_f=5
air_fm=20
h_vel=vector(8,0,0)
k=K*(N-1)
balls=[]
leftend=vector(0,0,0)
step=vector(0,-1,0)*L/(N-1)
s=L/(N-1)
R=L/30
for i in range(N):
    balls=balls+[sphere(pos=leftend+step*i, radius=R, m=M/N, p=vector(0,0,0), F=vector(0,0,0))]
springs=[]
for i in range(N-1):
    springs=springs+[helix(pos=leftend+i*step, radius=R/2, axis=step)]
t=0
dt=0.01
print(scene.camera.pos)
while(t<20):
    rate(100)
    scene.camera.pos=vector(balls[0].pos.x,balls[0].pos.y,10.0272)
    for i in range(1,N-1):
        balls[i].F=-k*(mag(springs[i-1].axis)-s)*norm(springs[i-1].axis)+k*(mag(springs[i].axis)-s)*norm(springs[i].axis)+balls[i].m*g-air_f*balls[i].p/balls[i].m
    balls[-1].F=-k*(mag(springs[-1].axis)-s)*norm(springs[-1].axis)+balls[-1].m*g-air_fm*balls[-1].p/balls[-1].m
    for ball in balls:
        if(ball!=balls[0]):
            ball.p=ball.p+ball.F*dt
            ball.pos=ball.pos+ball.p/ball.m*dt
        else:
            ball.pos=ball.pos+h_vel*dt
    for i in range (1,N):
        springs[i-1].axis=balls[i].pos-balls[i-1].pos
        springs[i-1].pos=balls[i-1].pos
    t=t+dt