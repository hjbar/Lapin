#include "jeu.hpp"

Jeu::Jeu()
	: pop{Population()}
	, grille{Grille()}
{
	int alea = 0;
	for(int i = 0; i < (TAILLEGRILLE * TAILLEGRILLE - 1); i++)
	{
		alea = int((rand() % 100) + 1);

		if(alea <= 20)
		{
			ajouteAnimal(Espece::Lapin, Coord(i));
		}
		else if(alea <= 27)
		{
			ajouteAnimal(Espece::Renard, Coord(i));
		}
	}
}

TEST_CASE("Constructeurs")
{

	Jeu game{};

	game.verifieGrille();

	int nbAnimalGrille = 0;
	int nbVideGrille = 0;

	int nbLapinPop = 0;
	int nbRenardPop = 0;
	int nbVidePop = 0;

	for(int i = 0; i < TAILLEGRILLE; i++)
	{
		for(int j = 0; j < TAILLEGRILLE; j++)
		{
			if(game.getGrille().getCase(Coord{i, j}) == -1)
			{
				nbVideGrille++;
			}
			else
			{
				nbAnimalGrille++;
			}
		}
	}

	for(long unsigned int i = 0; i < game.getPop().getT().size(); i++)
	{
		if(game.getPop().getT()[int(i)].getEspece() == Espece::Vide)
		{
			nbVidePop++;
			continue;
		}
		else if(game.getPop().getT()[int(i)].getEspece() == Espece::Lapin)
		{
			nbLapinPop++;
		}
		else
		{
			nbRenardPop++;
		}
	}

	CHECK(nbAnimalGrille == nbLapinPop + nbRenardPop);
	CHECK(nbVidePop == nbVideGrille);
	CHECK(nbLapinPop <= 100);
	CHECK(nbLapinPop >= 60);
	CHECK(nbRenardPop <= 40);
	CHECK(nbRenardPop >= 15);
}

Population Jeu::getPop() const
{
	return pop;
}

Grille Jeu::getGrille() const
{
	return grille;
}

void Jeu::ajouteAnimal(Espece e, Coord c)
{
	if(e == Espece::Vide)
	{
		grille.videCase(c);
		return;
	}
	Id ident = pop.reserve();
	grille.setCase(ident, c);
	Animal a{ident, e, c};
	pop.setAnimal(a);
}

TEST_CASE("Ajoute")
{

	Jeu j{};

	j.verifieGrille();

	int nbAnimauxAuDepart = j.getPop().getIds().cardinal();
	int nbCasesLibresAuDepart = int(j.getPop().getCasesLibres().size());

	if(j.getGrille().caseVide(Coord{1, 2}))
	{
		CHECK_THROWS_AS(j.getGrille().videCase(Coord{1, 2}), runtime_error);
	}
	else
	{
		j.getGrille().videCase(Coord{1, 2});
	}

	j.ajouteAnimal(Espece::Lapin, {1, 2});
	CHECK(j.getGrille().getCase(Coord{1, 2}) != -1);
	CHECK(j.getPop().getIds().cardinal() == nbAnimauxAuDepart + 1);
	CHECK(j.getPop().getCasesLibres().size() == nbCasesLibresAuDepart - 1);
	int ident = j.getGrille().getCase(Coord{1, 2});
	CHECK(j.getPop().get(ident).getEspece() == Espece::Lapin);

	j.verifieGrille();

	if(j.getGrille().caseVide(Coord{5, 4}))
	{
		CHECK_THROWS_AS(j.getGrille().videCase(Coord{5, 4}), runtime_error);
	}
	else
	{
		j.getGrille().videCase(Coord{5, 4});
	}

	j.ajouteAnimal(Espece::Renard, {5, 4});
	CHECK(j.getGrille().getCase(Coord{5, 4}) != -1);
	CHECK(j.getPop().getIds().cardinal() == nbAnimauxAuDepart + 2);
	CHECK(j.getPop().getCasesLibres().size() == nbCasesLibresAuDepart - 2);
	int ident2 = j.getGrille().getCase(Coord{5, 4});
	CHECK(j.getPop().get(ident2).getEspece() == Espece::Renard);

	j.verifieGrille();

	if(not j.getGrille().caseVide(Coord{6, 6}))
	{
		j.ajouteAnimal(Espece::Vide, {6, 6});
		CHECK(j.getGrille().getCase(Coord{6, 6}) == -1);
		CHECK(j.getPop().getIds().cardinal() == nbCasesLibresAuDepart + 2);
		CHECK(j.getPop().getCasesLibres().size() == nbCasesLibresAuDepart - 2);
		int ident3 = j.getGrille().getCase(Coord{6, 6});
		CHECK(j.getPop().get(ident3).getEspece() == Espece::Vide);

		j.verifieGrille();
	}
	else
	{
		CHECK_THROWS_AS(j.ajouteAnimal(Espece::Vide, {6, 6}), runtime_error);
	}
}

void Jeu::verifieGrille() const
{

	Ensemble ev = pop.getIds();
	Animal a{};

	for(int i = 0; i < ev.cardinal(); i++)
	{
		a = pop.get(ev.getCase(i));
		if(a.getId() != grille.getCase(a.getCoord()))
		{
			throw runtime_error(
				"La Grille et la Population ne sont pas correctement synchronis??es");
		}
	}
}
/**
TEST_CASE("VerifieGrille")
{
        Jeu j{};

        Ensemble ev = j.getPop().getIds();
        Animal a = j.getPop().get(ev.getCase(0));
        Coord cAvant = a.getCoord();

        int i = 14;
        Coord c{i, i * 24 % TAILLEGRILLE};
        do
        {
                i = i * 57 % TAILLEGRILLE;
                c = Coord{i, i * 24 % TAILLEGRILLE};
        } while(cAvant == c);
        a.setCoord(c);
        CHECK_THROWS_AS((j.verifieGrille()), runtime_error);

        j.getGrille().videCase(cAvant);
        j.getGrille().setCase(a.getId(), c);

        j.verifieGrille();
}
**/

Ensemble Jeu::voisinsVides(Coord c) const
{
	Ensemble voisin = c.voisines();
	Ensemble ev;
	for(int i = 0; i < voisin.cardinal(); i++)
	{
		if(grille.caseVide(Coord{voisin.getCase(i)}))
		{
			ev.ajoute(voisin.getCase(i));
		}
	}
	return ev;
}

Ensemble Jeu::voisinsAnimal(Espece e, Coord c) const
{
	if(e == Espece::Vide)
	{
		throw invalid_argument("Vide n'est pas un animal");
	}
	Ensemble voisin = c.voisines();
	Ensemble ev;
	int index = 0;
	for(int i = 0; i < voisin.cardinal(); i++)
	{
		index = grille.getCase(Coord{voisin.getCase(i)});
		if(index == -1)
		{
			continue;
		}
		if(pop.get(index).getEspece() == e)
		{
			ev.ajoute(voisin.getCase(i));
		}
	}
	return ev;
}

TEST_CASE("VoisinsVides + VoisinsAnimal")
{
	Jeu game{};

	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			if(not game.getGrille().caseVide(Coord{i, j}))
			{
				game.ajouteAnimal(Espece::Vide, Coord{i, j});
			}
		}
	}

	CHECK_THROWS_AS(game.voisinsAnimal(Espece::Vide, Coord({1, 1})), invalid_argument);

	Ensemble ev1 = game.voisinsVides(Coord{1, 1});
	Ensemble el1 = game.voisinsAnimal(Espece::Lapin, Coord{1, 1});
	Ensemble er1 = game.voisinsAnimal(Espece::Renard, Coord{1, 1});

	CHECK(ev1.cardinal() == 8);
	CHECK(el1.cardinal() == 0);
	CHECK(er1.cardinal() == 0);

	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			game.ajouteAnimal(Espece::Lapin, Coord{i, j});
		}
	}

	Ensemble ev2 = game.voisinsVides(Coord{1, 1});
	Ensemble el2 = game.voisinsAnimal(Espece::Lapin, Coord{1, 1});
	Ensemble er2 = game.voisinsAnimal(Espece::Renard, Coord{1, 1});

	CHECK(ev2.cardinal() == 0);
	CHECK(el2.cardinal() == 8);
	CHECK(er2.cardinal() == 0);

	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			game.ajouteAnimal(Espece::Vide, Coord{i, j});
			game.ajouteAnimal(Espece::Renard, Coord{i, j});
		}
	}

	Ensemble ev3 = game.voisinsVides(Coord{1, 1});
	Ensemble el3 = game.voisinsAnimal(Espece::Lapin, Coord{1, 1});
	Ensemble er3 = game.voisinsAnimal(Espece::Renard, Coord{1, 1});

	CHECK(ev3.cardinal() == 0);
	CHECK(el3.cardinal() == 0);
	CHECK(er3.cardinal() == 8);
}

void Jeu::deplaceAnimal(Animal a)
{

	Espece e = a.getEspece();

	if(e == Espece::Vide)
	{
		throw runtime_error("On ne peut pas deplacer un Animal vide");
	}

	else if(e == Espece::Lapin)
	{
		deplaceLapin(a);
		return;
	}

	else if(e == Espece::Renard)
	{
		a.jeune();
		if(a.meurt())
		{
			supprimeAnimal(a);
			return;
		}
		deplaceRenard(a);
	}

	else
	{
		throw runtime_error("Cet espece d'Animal n'existe pas");
	}
}

void Jeu::deplaceLapin(Animal a)
{
	Coord cDepart = a.getCoord();
	Ensemble e = voisinsVides(cDepart);

	if(e.cardinal() == 0)
	{
		return;
	}

	int indice = e.tire();
	Coord cFin{indice};

	deplaceCoord(a, cFin);

	e.ajoute(indice);
	naissance(a, cDepart, e);
}

void Jeu::deplaceRenard(Animal a)
{
	Coord cDepart = a.getCoord();
	Ensemble eVide = voisinsVides(cDepart);
	Ensemble eLapin = voisinsAnimal(Espece::Lapin, cDepart);

	if(eLapin.cardinal() > 0)
	{
		int indice = eLapin.tire();
		Coord cFin{indice};
		Animal lapin = pop.get(grille.getCase(cFin));
		supprimeAnimal(lapin);
		a.mange();

		deplaceCoord(a, cFin);
	}

	else if(eVide.cardinal() > 0)
	{
		int indice = eVide.tire();
		Coord cFin{indice};

		deplaceCoord(a, cFin);
	}

	else
	{
		return;
	}

	naissance(a, cDepart);
}

void Jeu::naissance(Animal a, Coord c, Ensemble ens)
{

	bool reproduction = a.seReproduit(ens.cardinal());

	if(reproduction)
	{
		ajouteAnimal(a.getEspece(), c);
	}
}

void Jeu::naissance(Animal a, Coord c)
{

	bool reproduction = a.seReproduit();

	if(reproduction)
	{
		ajouteAnimal(a.getEspece(), c);
	}
}

void Jeu::supprimeAnimal(Animal a)
{
	Coord c = a.getCoord();

	grille.videCase(c);
	pop.supprime(a);
}

TEST_CASE("supprimeAnimal")
{
	Jeu j{};

	Ensemble e = j.getPop().getIds();
	Animal a{};

	for(int i = 0; i < e.cardinal(); i++)
	{

		a = j.getPop().get(e.getCase(i));

		Id idA = a.getId();
		Coord cA = a.getCoord();
		Espece espA = a.getEspece();

		CHECK(j.getPop().get(idA).getEspece() == espA);
		CHECK(j.getPop().get(idA).getCoord() == cA);
		CHECK(j.getPop().get(idA).getId() == idA);
		CHECK(j.getGrille().getCase(cA) == idA);

		j.supprimeAnimal(a);

		CHECK_THROWS_AS(j.getPop().get(idA), invalid_argument);
		CHECK(j.getGrille().getCase(cA) == -1);
	}
}

void Jeu::deplaceCoord(Animal a, Coord cFin)
{
	Coord cDepart = a.getCoord();
	grille.setCase(a.getId(), cFin);
	pop.changeCoord(a, cFin);
	grille.videCase(cDepart);
}

TEST_CASE("deplaceCoord")
{
	Jeu jeu{};

	Ensemble e = jeu.getPop().getIds();
	Coord c{0, 0};

	for(int i = 0; i < TAILLEGRILLE; i++)
	{
		for(int j = 0; j < TAILLEGRILLE; j++)
		{
			if(jeu.getGrille().getCase(Coord{i, j}) == -1)
			{
				c = Coord{i, j};
				break;
			}
		}
	}

	Animal a = jeu.getPop().get(e.getCase(0));

	Coord cAvant = a.getCoord();
	jeu.deplaceCoord(a, c);

	Animal newA = jeu.getPop().get(e.getCase(0));
	Coord cApres = newA.getCoord();

	CHECK_FALSE(cAvant == c);
	CHECK_FALSE(cApres == cAvant);
	CHECK(cApres == c);
	CHECK(jeu.getGrille().getCase(cAvant) == -1);
	CHECK(jeu.getGrille().getCase(c) == newA.getId());
}

void Jeu::affiche() const
{
	cout << NEW_SCREEN;
	for(int i = 0; i < TAILLEGRILLE; i++)
	{
		for(int j = 0; j < TAILLEGRILLE; j++)
		{
			if(grille.getCase(Coord{i, j}) == -1)
			{
				cout << Animal{}.toString() << "   ";
			}
			else
			{
				cout << pop.get(grille.getCase(Coord{i, j})).toString() << "   ";
			}
		}
		cout << endl << endl;
	}
	afficheNombreAnimal();
}

void Jeu::afficheNombreAnimal() const
{
	int nbLapin = 0;
	int nbRenard = 0;
	Ensemble e = pop.getIds();

	for(int i = 0; i < e.cardinal(); i++)
	{
		if(pop.get(e.getCase(i)).getEspece() == Espece::Lapin)
		{
			nbLapin++;
		}
		else if(pop.get(e.getCase(i)).getEspece() == Espece::Renard)
		{
			nbRenard++;
		}
	}

	cout << "Lapin : " << nbLapin << "   "
		 << "Renard : " << nbRenard << endl;
}

void Jeu::verifieAll() const
{

	verifieGrille();

	Coord cGrille{0, 0};
	Coord cPop{0, 0};
	for(int i = 0; i < TAILLEGRILLE; i++)
	{
		for(int j = 0; j < TAILLEGRILLE; j++)
		{
			if(grille.getCase(Coord{i, j}) != -1)
			{
				cGrille = Coord{i, j};
				cPop = pop.get(grille.getCase(Coord{i, j})).getCoord();
				if(not(cPop == cGrille))
				{
					throw runtime_error("L'animal n'est pas stocke a la place ou il doit l'etre");
				}
			}
		}
	}

	Ensemble e = pop.getIds();
	Coord cAnimal{0, 0};
	for(int i = 0; i < e.cardinal(); i++)
	{
		cAnimal = pop.get(e.getCase(i)).getCoord();
		if(grille.getCase(cAnimal) == -1)
		{
			throw runtime_error("L'animal est stocke dans une case vide");
		}
	}

	bool ok = false;
	Ensemble ev = pop.getIds();
	for(int i = 0; i < TAILLEGRILLE; i++)
	{
		for(int j = 0; j < TAILLEGRILLE; j++)
		{
			if(grille.getCase(Coord{i, j}) != -1)
			{
				for(int k = 0; k < ev.cardinal(); k++)
				{
					if(grille.getCase(Coord{i, j}) == ev.getCase(k))
					{
						if(not ok)
						{
							ok = true;
						}
					}
				}
				if(not ok)
				{
					throw runtime_error("L'animal stocke n'est pas dans la population");
				}
			}
		}
	}
}
