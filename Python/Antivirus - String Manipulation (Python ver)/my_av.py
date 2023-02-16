#!/usr/bin/python3
import re
#functii task1
#functie care verifica lungimea domeniului
def test_domain_length(URL):
    li = list(URL.split("/"))
    domain = li[0]
    verif = 0
    if len(domain) > 30:
        verif = 1
        #print(domain)
    return verif
#functie care testeaza raportul de cifre si litere din domeniu
def test_domain_ratio(URL):
    li = list(URL.split("/"))
    domain = li[0]
    numbers = 0
    letters = 0
    for i in range(len(domain)):
        if domain[i - 1].isnumeric():
            numbers = numbers + 1
        elif domain[i - 1].isalpha():
            letters = letters + 1
    ratio = 1
    if letters != 0:
        ratio = numbers / letters
    #print(domain, ratio)
    verif = 0
    if(ratio > 0.64):
        verif = 1
    return verif
#functie care testeaza daca domeniul apartine listei domains_database
def test_domain(URL):
    li = list(URL.split("/"))
    domain = li[0]
    domains_file = open("./data/urls/domains_database", "r")
    domains_line = domains_file.readline()
    verif = 0
    while domains_line:
        domains_line = domains_line[:-1]
        if domains_line == domain:
            verif = 1
            break
        domains_line = domains_file.readline()
    return verif
#functie care verifica aparitia anumitor cuvinte in URL
def test_words(URL):
    words_file = open("./words", "r")
    word = words_file.readline()
    verif = 0
    while word:
        word = word[:-1]
        #print(word)
        if word in URL:
            verif = 1
        word = words_file.readline()
    return verif
#functie care verifica extensia de la sfarsitul URL-urilor
def test_extension(URL):
    verif = 0
    if '/' in URL:
        li1 = list(URL.split("/"))
        last_atribute = li1[-1]
        if '.' in last_atribute:
            li2 = list(last_atribute.split("."))
            extension = li2[1]
            if extension == 'exe' or extension == 'js' or extension == '.py' or extension == '.js':
                verif = 1
    elif '.' in URL:
        li2 = list(URL.split("."))
        extension = li2[1]
        if extension == 'exe' or extension == 'js' or extension == '.py' or extension == '.js':
            verif = 1
        #print(last_atribute, file = exitfile)
    return verif
#functie care verifica daca domeniul este sub forma unei adrese IP
def test_IP(URL):
    li = list(URL.split("/"))
    domain = li[0]
    verif = 0
    x = re.search("(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.)"
				  "{3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])", domain)
    if x:
        #print(domain)
        verif = 1
    return verif
#functii task 2
#functie care returneaza valoarea atributului flow_druation din traffic
def flow_duration(traffic):
    traffic_list = list(traffic.split(","))
    flow_duration = traffic_list[4]
    flow_duration_list = re.split(" |:", flow_duration)
    seconds = float(flow_duration_list[0]) * 86400
    seconds = seconds + float(flow_duration_list[2]) * 3600
    seconds = seconds + float(flow_duration_list[3]) * 60
    seconds = seconds + float(flow_duration_list[4])
    #print(flow_duration_list, seconds)
    return seconds
#functie care verifica daca traficul este de tip bruteforce
def test_bruteforce(traffic):
    seconds = flow_duration(traffic)
    verif = 0
    if seconds >= 1.8 and seconds <=2.5:
        traffic_list = list(traffic.split(","))
        flow_payload = float(traffic_list[16])
        print(flow_payload)
        if flow_payload > 30:
            verif = 1
    return verif
#functie care verifica daca traficul este de tip cryptominer
def test_cryptominer(traffic):
    traffic_list = list(traffic.split(","))
    flow_payload = float(traffic_list[16])
    verif = 0
    if flow_payload > 0:
        bwd = float(traffic_list[15])
        if bwd == 0:
            seconds = flow_duration(traffic)
            if seconds == 0 or (seconds > 1.5 and seconds < 20):
                verif = 1
    return verif
#task1
file_URL = open("./data/urls/urls.in", "r")
exitfile = open("./urls-predictions.out", "w")
#fiecare URL din fisier va fi stocat pe rand in string-ul URL
URL = file_URL.readline()
while URL:
    URL = URL[:-1]
    verif = 0
    verif = test_domain(URL)
    if verif == 0:
        verif = test_extension(URL)
    if verif == 0:
        verif = test_IP(URL)
    if verif == 0:
        verif = test_domain_length(URL)
    if verif == 0:
        verif = test_domain_ratio(URL)
    if verif == 0:
        verif = test_words(URL)
    print(verif, file = exitfile)
    URL = file_URL.readline()
exitfile.close()
file_URL.close()
#task2
file_traffic = open("./data/traffic/traffic.in", "r")
exitfile = open("./traffic-predictions.out", "w")
#fiecare trafic din fisier va fi stocat pe rand in string-ul traffic
traffic = file_traffic.readline() #scapam de prima linie
traffic = file_traffic.readline()
while traffic:
    traffic = traffic[:-1]
    traffic_list = list(traffic.split(","))
    verif = 0
    verif = test_bruteforce(traffic)
    if verif == 0:
        verif = test_cryptominer(traffic)
    print(verif, file = exitfile)
    traffic = file_traffic.readline()
exitfile.close()
file_traffic.close()
#print(data1)
#print("a")
#for line in data:
    