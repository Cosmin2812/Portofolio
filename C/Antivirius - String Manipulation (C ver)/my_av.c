#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
// functii task 1
// functie care verifica lungimea domeniului
int test_domain_length(char *URL)
{
	char *domain = calloc(200, sizeof(char));
	if (strchr(URL, '/'))
		memcpy(domain, URL, strchr(URL, '/') - URL);
	else
		memcpy(domain, URL, strlen(URL));
	int verif = 0;
	if (strlen(domain) > 30)
		verif = 1;
	free(domain);
	return verif;
}

// functie care testeaza raportul de cifre si litere din domeniu
int test_domain_ratio(char *URL)
{
	char *domain = calloc(200, sizeof(char));
	if (strchr(URL, '/'))
		memcpy(domain, URL, strchr(URL, '/') - URL);
	else
		memcpy(domain, URL, strlen(URL));
	int letters = 0;
	int numbers = 0;
	for (int i = 0; i <= strlen(domain) - 1; i++) {
		if (isalpha(domain[i]))
			letters++;
		else if (isdigit(domain[i]))
			numbers++;
	}
	float ratio = (float)numbers / letters;
	int verif = 0;
	if (ratio > 0.64)
		verif = 1;
	free(domain);
	return verif;
}

// functie care verifica aparitia anumitor cuvinte in URL
int test_words(char *URL)
{
	FILE *file_pointer_words = fopen("./words", "r");
	char *word = calloc(20, sizeof(char));
	int verif = 0;
	while (fgets(word, 19, file_pointer_words)) {
		word[strlen(word) - 1] = 0;
		if (strstr(URL, word)) {
			verif = 1;
			break;
		}
	}
	fclose(file_pointer_words);
	free(word);
	return verif;
}

// functie care verifica daca domeniul este sub forma unei adrese IP
int test_IP(char *URL)
{
	regex_t IP_check;
	int value = regcomp(&IP_check,
						"(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.)"
						"{3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])",
						REG_EXTENDED);
	int verif = regexec(&IP_check, URL, 0, NULL, 0);
	regfree(&IP_check);
	if (verif == 0)
		return 1;
	else
		return 0;
}

// functie care testeaza daca domeniul apartine listei domains_database
int test_domain(char *URL)
{
	FILE *file_pointer_temp;
	char *domain = calloc(200, sizeof(char));
	if (strchr(URL, '/'))
		memcpy(domain, URL, strchr(URL, '/')  - URL);
	else
		memcpy(domain, URL, strlen(URL));
	char *temp_domain = calloc(200, sizeof(char));
	int verif = 0;
	file_pointer_temp = fopen("./data/urls/domains_database", "r");
	while (fgets(temp_domain, 200, file_pointer_temp)) {
		temp_domain[strlen(temp_domain) - 1] = 0;
		if (!strcmp(temp_domain, domain)) {
			verif = 1;
			break;
		}
	}
	fclose(file_pointer_temp);
	free(domain);
	free(temp_domain);
	return verif;
}

// functie care verifica extensia de la sfarsitul URL-urilor
int test_extension(char *URL)
{
	char *temp1 = calloc(5, sizeof(char));
	temp1 = memcpy(temp1, URL + strlen(URL) - 4, 4);
    // printf("%s\n", temp);
	int verif = 0;
	if (!strcmp(temp1, ".exe") || !strcmp(temp1, ".pdf"))
		verif = 1;
	free(temp1);
	char *temp2 = calloc(4, sizeof(char));
	temp2 = memcpy(temp2, URL + strlen(URL) - 3, 3);
	if (!strcmp(temp2, ".py") || !strcmp(temp2, ".sh") || !strcmp(temp2, ".js"))
		verif = 1;
	free(temp2);
	return verif;
}

// functii task 2
// functie care returneaza valoarea atributului flow_druation din traffic
double flow_duration(char *traffic)
{
	char *temp;
	temp = strchr(traffic, ',');
	temp++;
	for (int i = 1; i <= 3; i++) {
		temp = strchr(temp, ',');
		temp++;
	}
	double seconds = atof(temp) * 86400;
	temp = strchr(traffic, ' ');
	temp++;
	temp = strchr(temp, ' ');
	temp++;
	seconds = seconds + atof(temp) * 3600;
	temp = strchr(temp, ':');
	temp++;
	seconds = seconds + atof(temp) * 60;
	temp = strchr(temp, ':');
	temp++;
	seconds = seconds + atof(temp);
	return seconds;
}

// functie care verifica daca traficul este de tip bruteforce
int test_bruteforce(char *traffic)
{
	double seconds = flow_duration(traffic);
	int verif = 0;
	if (seconds >= 1.8 && seconds <= 2.5) {
		char *temp;
		temp = strchr(traffic, ',');
		temp++;
		for (int i = 1; i <= 15; i++) {
			temp = strchr(temp, ',');
			temp++;
		}
		double flow_payload = atof(temp);
		if (flow_payload > 30)
			verif = 1;
	}
    // printf("%lf\n", seconds);
	return verif;
}

// functie care verifica daca traficul este de tip cryptominer
int test_cryptominer(char *traffic)
{
	char *temp;
	temp = strchr(traffic, ',');
	temp++;
	for (int i = 1; i <= 15; i++) {
		temp = strchr(temp, ',');
		temp++;
	}
	double flow_payload = atof(temp);
	int verif = 0;
	if (flow_payload > 0) {
		temp = strchr(traffic, ',');
		temp++;
		for (int i = 1; i <= 14; i++) {
			temp = strchr(temp, ',');
			temp++;
		}
		double bwd = atof(temp);
		if (bwd == 0) {
			double seconds = flow_duration(traffic);
			if (seconds == 0 || (seconds > 1.5 && seconds < 20))
				verif = 1;
		}
	}
	return verif;
}

int main(void)
{
	// task 1
	FILE *file_pointer, *file_pointer_temp, *file_pointer_out;
	FILE *file_verif_pointer, *file_classes_pointer;
	file_verif_pointer = fopen("./verification", "w");
	file_classes_pointer = fopen("./data/urls/urls_classes", "r");
	file_pointer_out = fopen("./urls-predictions.out", "w");
	file_pointer = fopen("./data/urls/urls.in", "r");
	// fiecare URL din fisier va fi stocat pe rand in string-ul URL
	char *URL = calloc(800, sizeof(char));
	while (fgets(URL, 799, file_pointer)) {
		URL[strlen(URL) - 1] = 0;
		int verif = test_domain(URL);
		if (verif == 0)
			verif = test_extension(URL);
		if (verif == 0)
			verif = test_words(URL);
		if (verif == 0)
			verif = test_IP(URL);
		if (verif == 0)
			verif = test_domain_length(URL);
		if (verif == 0)
			verif = test_domain_ratio(URL);
		fprintf(file_pointer_out, "%d\n", verif);
	}
	fclose(file_pointer);
	fclose(file_pointer_out);
	free(URL);
	// task 2
	FILE *file_pointer_traffic;
	file_pointer_out = fopen("./traffic-predictions.out", "w");
	file_pointer_traffic = fopen("./data/traffic/traffic.in", "r");
	// fiecare trafic din fisier va fi stocat pe rand in vectorul traffic
	char *traffic = calloc(300, sizeof(char));
    // scapam de prima linie
	fgets(traffic, 299, file_pointer_traffic);
	while (fgets(traffic, 299, file_pointer_traffic)) {
		traffic[strlen(traffic) - 1] = 0;
		int verif = 0;
		verif = test_bruteforce(traffic);
		if (verif == 0)
			verif = test_cryptominer(traffic);
		fprintf(file_pointer_out, "%d\n", verif);
	}
	fclose(file_pointer_traffic);
	fclose(file_pointer_out);
	free(traffic);
}
