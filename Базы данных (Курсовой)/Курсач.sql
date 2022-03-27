--1
create table if not exists Countries
(
	id serial primary key,
	name text
);

--2
create table if not exists Sponsors
(
	id serial primary key,
	country_id int,
	FOREIGN KEY (country_id) REFERENCES public.Countries (id),
	name text	
);

--CREATE TABLE IF NOT EXISTS GameRoles

--3
create table if not exists Teams
(
	id serial primary key,
	country_id int,
	sponsor_id int,
	FOREIGN KEY (country_id) REFERENCES public.Countries (id),
	FOREIGN KEY (sponsor_id) REFERENCES public.Sponsors (id),
	name text
);

--4
create table if not exists Players
(
	id serial primary key,
	team_id int,
	country_id int,
	FOREIGN KEY (team_id) REFERENCES public.Teams (id),
	FOREIGN KEY (country_id) REFERENCES public.Countries (id),
	nickname text,
	first_name text,
	second_name text,
	birth_year int,
	main_role text,
	rating int
);

--5
create table if not exists Studios
(
	id serial primary key,
	country_id int,
	FOREIGN KEY (country_id) REFERENCES public.Countries (id),
	name text
);

--6
create table if not exists Commentators
(
	id serial primary key,
	studio_id int,
	country_id int,
	FOREIGN KEY (studio_id) REFERENCES public.Studios (id),
	FOREIGN KEY (country_id) REFERENCES public.Countries (id),
	nickname text,
	first_name text,
	second_name text,
	birth_year int,
	popularity int
);

--7
create table if not exists Tournaments
(
	id serial primary key,
	country_id int,
	FOREIGN KEY (country_id) REFERENCES public.Countries (id),
	name text,
	prizepool int
);

--8
create table if not exists Matches
(
	team1_id int,
	team2_id int,
	winner_id int,
	studio_id int,
	commentator_id int,
	tournament_id int,
	FOREIGN KEY (team1_id) REFERENCES public.Teams (id),
	FOREIGN KEY (team2_id) REFERENCES public.Teams (id),
	FOREIGN KEY (winner_id) REFERENCES public.Teams (id),
	FOREIGN KEY (studio_id) REFERENCES public.Studios (id),
	FOREIGN KEY (commentator_id) REFERENCES public.Commentators (id),
	FOREIGN KEY (tournament_id) REFERENCES public.Tournaments (id),
	date date
);

--9
--Admin, Tournaments Manager, Tournament Organizer, Studio Owner, Team Owner
create table if not exists UserRoles
(
	id serial primary key,
	name text
);

--10
create table if not exists Users
(
	id serial primary key,
	role_id int,
	FOREIGN KEY (role_id) REFERENCES public.UserRoles (id),
	--email text,
	login text,
	password text
);





