#include <stdlib.h>
#include <vector>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

void NoeudSeqInst::traduitEnJava(ostream & cout, unsigned int indentation) const {
    for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->traduitEnJava(cout, 0);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudAffectation::traduitEnJava(ostream & cout, unsigned int indentation) const {
    m_variable->traduitEnJava(cout,0);
    cout << " = ";
    m_expression->traduitEnJava(cout,0);
    cout << ";"<<endl;
    
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}


void NoeudOperateurBinaire::traduitEnJava(ostream & cout, unsigned int indentation) const {
    int og, od, valeur;
    if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
    if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
    if (    this->m_operateur == "+" || this->m_operateur == "-" || this->m_operateur == "*" ||
            this->m_operateur == "==" || this->m_operateur == "!=" || this->m_operateur == "<" ||
            this->m_operateur == ">" || this->m_operateur == "<=" || this->m_operateur == ">=") {
        char* ogStr; sprintf(ogStr,"%d",og);
        char* odStr; sprintf(odStr,"%d",od);
        cout << setw(4*indentation) << "" << "val = (" << ogStr << m_operateur.getChaine() << odStr << ");";
    }
    else if (this->m_operateur == "et") cout << setw(4*indentation) << "" << "val = (" << og << "&&" << od << ");";
    else if (this->m_operateur == "ou") cout << setw(4*indentation) << "" << "val = (" << og << "||" << od << ");";
    else if (this->m_operateur == "non") cout << setw(4*indentation) << "" << "val = (" << "!" << og << ");";
    else if (this->m_operateur == "/") {
      if (od != 0) {
        cout << setw(4*indentation) << "" << "val = (" << og << "/" << od << ");";
      }
      else {
          cout << setw(4*indentation) << "" << "Erreur, division par 0 impossible";
      }
    }

}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
  if (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}


void NoeudInstSi::traduitEnJava(ostream & cout, unsigned int indentation) const {
    cout << setw(4*indentation) <<""<<"if ("; //ecrit "if (" avec 4 espaces
    m_condition->traduitEnJava(cout, 0); //traduit la condition en java, pas de décalage
    cout << ") {"<<endl; //ecrit ") {" et va a la ligne
    m_sequence->traduitEnJava(cout, indentation+1); //traduit en java la séquence avec indentation +1
    cout << setw(4*indentation) << "" << "" << "}" << endl; //ecrit "}" avec l'indentation initiale et passe a la ligne
    
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence):m_condition(condition),m_sequence(sequence) {}

int NoeudInstTantQue::executer(){
    while(m_condition->executer()) m_sequence->executer();
    return 0;
}

void NoeudInstTantQue::traduitEnJava(ostream & cout, unsigned int indentation) const {
    cout << setw(4*indentation)<<""<<"while ("; //ecrit for ( avec 4 espaces
    m_condition->traduitEnJava(cout, 0); //traduit la condition sans décalage
    cout << ") {"<<endl; //ecrit ) { et va a la ligne
    m_sequence->traduitEnJava(cout, indentation+1); //traduit la sequence d'instructions avec 1 espace de décalage
    cout << setw(4*indentation)<<""<<"}"<<endl; //ecrit } avec l'indentation initiale et va a la ligne
    
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* sequence, Noeud* condition):m_sequence(sequence), m_condition(condition) {}

int NoeudInstRepeter::executer(){
    do {
        m_sequence->executer();
    } while (m_condition->executer());
    return 0;
}


void NoeudInstRepeter::traduitEnJava(ostream & cout, unsigned int indentation) const {
    cout << setw(4*indentation) << "" << "do {"<<endl;
    m_sequence->traduitEnJava(cout, indentation+1);
    cout<<setw(4*indentation)<<""<<"}"<<" while (";
    m_condition->traduitEnJava(cout, 0);
    cout<<""<<" );"<<endl;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* initialisation, Noeud* condition, Noeud* iteration, Noeud* sequence): m_initialisation(initialisation), m_condition(condition), m_iteration(iteration), m_sequence(sequence) {}

int NoeudInstPour::executer() {
    for(m_initialisation->executer();m_condition->executer();m_iteration->executer()) m_sequence->executer();
    return 0;
}

void NoeudInstPour::traduitEnJava(ostream & cout, unsigned int indentation) const {
    cout<<setw(4*indentation)<<""<<"for (";
    m_initialisation->traduitEnJava(cout, 0);
    cout<<";";
    m_condition->traduitEnJava(cout, 0 );
    cout<<";";
    m_iteration->traduitEnJava(cout,0);
    cout<<""<<" )"<<" {"<<endl;
    m_sequence->traduitEnJava(cout,indentation+1);
    cout<<""<<" }"<<endl;
}


////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire(Noeud* chaine): m_chaines() {
    this->ajoute(chaine);
}

int NoeudInstEcrire::executer() {
    string res;
    
        for (unsigned int i = 0; i < m_chaines.size(); i++) {
            if ( (typeid(*m_chaines[i])==typeid(SymboleValue) &&  *((SymboleValue*)m_chaines[i])== "<CHAINE>" )){
                res = res + ((SymboleValue*)m_chaines[i])->getChaine().substr(1,((SymboleValue*)m_chaines[i])->getChaine().size()-2);
            } else {
                res = res + to_string(m_chaines[i]->executer());
            }
        }
        cout << res <<"\n";
    return 0;
}

void NoeudInstEcrire::ajoute(Noeud * chaine){
    if (chaine!=nullptr) m_chaines.push_back(chaine);
}

void NoeudInstEcrire::traduitEnJava(ostream & cout, unsigned int indentation) const {
  cout<<"System.out.print(\" ";  
  for (unsigned int i = 0; i < m_chaines.size(); i++) {
            if ( (typeid(*m_chaines[i])==typeid(SymboleValue) &&  *((SymboleValue*)m_chaines[i])== "<CHAINE>" )){
                cout << ((SymboleValue*)m_chaines[i])->getChaine().substr(1,((SymboleValue*)m_chaines[i])->getChaine().size()-2);
            } else {
                cout << to_string(m_chaines[i]->executer()) ;
            }
            cout << " \"+\" ";
        }
  cout<<" \"); ";
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire(Noeud* variable) : m_variables() {
    this->ajoute(variable);
}

int NoeudInstLire::executer() {
        for (unsigned int i = 0; i < m_variables.size(); i++){
            int valeur;
            cin>>valeur;
            ((SymboleValue*)m_variables[i])->setValeur(valeur);
        }
        return 0;
}

void NoeudInstLire::ajoute(Noeud * variable){
    if (variable!=nullptr) m_variables.push_back(variable);
}

void NoeudInstLire::traduitEnJava(ostream & cout, unsigned int indentation) const {
     //on définira Scanner sc = new Scanner(System.in);
     //ainsi que int read; afin d'éviter les erreurs
    for (unsigned int i = 0; i < m_variables.size(); i++){
            string read = ((SymboleValue*)m_variables[i])->getChaine();
            cout <<"int"<< read << "= sc.nextLine();" << endl;
         
        }
            
}

    
