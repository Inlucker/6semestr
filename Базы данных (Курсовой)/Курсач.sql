drop table public.Countries cascade;
drop table public.Sponsors cascade;
drop table public.Teams cascade;
drop table public.Players cascade;
drop table public.Studios cascade;
drop table public.Commentators cascade;
drop table public.Tournaments cascade;
drop table public.Matches cascade;
drop table public.UserRoles cascade;
drop table public.Users cascade;
drop table public.TournamentTeams cascade;


--9
--Admin, Tournament Organizer, Studio Owner, Team Captain
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
	login text,
	password text
);

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
	captain_id int,
	FOREIGN KEY (country_id) REFERENCES public.Countries (id),
	FOREIGN KEY (sponsor_id) REFERENCES public.Sponsors (id),
	FOREIGN KEY (captain_id) REFERENCES public.Users (id),
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
	owner_id int,
	FOREIGN KEY (country_id) REFERENCES public.Countries (id),
	FOREIGN KEY (owner_id) REFERENCES public.Users (id),
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
	organizer_id int,
	FOREIGN KEY (country_id) REFERENCES public.Countries (id),
	FOREIGN KEY (organizer_id) REFERENCES public.Users (id),
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


--11
create table if not exists TournamentTeams
(
	tournament_id int,
	team_id int,
	FOREIGN KEY (tournament_id) REFERENCES public.Tournaments (id),
	FOREIGN KEY (team_id) REFERENCES public.Teams (id)
);


insert into countries values(1, 'Russia');
insert into countries values(2, 'USA');
insert into countries values(3, 'Ukraine');

insert into sponsors values(1, 1, 'GGBet');
insert into sponsors values(2, 2, 'Red Bull');
insert into sponsors values(3, 2, 'Coca-cola');

insert into teams values(1, 1, 1, 'Virtus Pro');
insert into teams values(2, 2, 2, 'Evil Geniuses');
insert into teams values(3, 3, 3, 'Natus Vincere');

insert into players values(1, 1, 1, 'GPK', 'Danil', 'Skutin', 2001, 'Midlaner', 12000);
insert into players values(2, 2, 1, 'Nightfall', 'Egor', 'Grigorenko', 2002, 'Offlaner', 11000);
insert into players values(3, 3, 3, 'Noone', 'Vladimir', 'Minenko', 1997, 'Midlaner', 10000);

insert into studios values(1, 1, 'RuHub');
insert into studios values(2, 2, 'Beyond the Summit');
insert into studios values(3, 3, 'Maincast');

insert into commentators values(1, 1, 1, '4ce', 'Dmitriy', 'Filinov', 1991, 10000);
insert into commentators values(2, 2, 2, 'Forsaken Oracle', 'Kyle', 'Freedman', 1993, 11000);
insert into commentators values(3, 3, 3, 'ALWAYSWANNAFLY', 'Andrey', 'Bondarenko', 1991, 12000);

insert into tournaments values(1, 2, 'The Inernational 2021', 40018195);

insert into matches values(1, 2, 1, 1, 1, 1, '2021-11-07');
insert into matches values(2, 3, 2, 2, 2, 1, '2021-11-08');
insert into matches values(3, 1, 3, 3, 3, 1, '2021-11-09');

--Все матчи с конкретным комментатором
select *
from matches
where commentator_id = 2

--Все команды которые выиграли хотябы 1 матч
select id, name
from teams t
where exists (select id
			  from matches m
			  where t.id = m.winner_id)
			 
--Игркои чей рейтинг больше 11к
select nickname, rating
from players
where rating > 11000

--Средние, минимальные и максимальные возраста для ролей
select distinct main_role,
	avg(extract(year from CURRENT_DATE) - birth_year) over(partition by p.main_role) as AvgAge,
	min(extract(year from CURRENT_DATE) - birth_year) over(partition by p.main_role) as MinAge,
	max(extract(year from CURRENT_DATE) - birth_year) over(partition by p.main_role) as MaxAge
from players p 