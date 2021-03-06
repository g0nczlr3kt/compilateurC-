#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr) {
}

void Interpreteur::analyse() {
  m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::tester(const string & symboleAttendu) const throw (SyntaxeException) {
  // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
  static char messageWhat[256];
  if (m_lecteur.getSymbole() != symboleAttendu) {
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s\n",
            m_lecteur.getLigne(), m_lecteur.getColonne(),
            symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
  }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException) {
  // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
  tester(symboleAttendu);
  m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const throw (SyntaxeException) {
  // Lève une exception contenant le message et le symbole courant trouvé
  // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
  static char messageWhat[256];
  sprintf(messageWhat,
          "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
          m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
  throw SyntaxeException(messageWhat);
}



Noeud* Interpreteur::programme() {
  // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
  testerEtAvancer("procedure");
  testerEtAvancer("principale");
  testerEtAvancer("(");
  testerEtAvancer(")");
  Noeud* sequence = seqInst();
  int tryHard = 0; 
  do {
    try{
    testerEtAvancer("finproc");
    tryHard = 0;
    }
    catch (SyntaxeException e) {
          cout << e.what();
          while (m_lecteur.getSymbole() != "si"  && m_lecteur.getSymbole() != "<VARIABLE>" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole() != "finproc" && m_lecteur.getSymbole() != "finsi" && m_lecteur.getSymbole() != "finpour" && m_lecteur.getSymbole() != "fintantque" && m_lecteur.getSymbole() != "<FINDEFICHIER>")
            {
                m_lecteur.avancer();
            }
          if (m_lecteur.getSymbole() == "<FINDEFICHIER>") {
              cout << "finproc manquant !";
              tryHard = 0;
          }
          else if (m_lecteur.getSymbole() == "finproc") {
              tryHard = 0;
          }
          else {
                while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si" || m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "repeter" || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "ecrire" || m_lecteur.getSymbole() == "lire" )
                {
                  ((NoeudSeqInst*) sequence)->ajoute(inst());
                }
              tryHard = 1;
          }
      }
  }while (tryHard == 1);
  try{
  tester("<FINDEFICHIER>");
  }
  catch(SyntaxeException e){
      cout << e.what();
  }
  return sequence;
    
}

Noeud* Interpreteur::seqInst() {
  // <seqInst> ::= <inst> { <inst> }
  NoeudSeqInst* sequence = new NoeudSeqInst();
  do {
    sequence->ajoute(inst());
  } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si" || m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "repeter" || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "ecrire" || m_lecteur.getSymbole() == "lire" );
  // Tant que le symbole courant est un début possible d'instruction...
  // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
  return sequence;
}

Noeud* Interpreteur::inst() {
  // <inst> ::= <affectation>  ; | <instSi>
    try {
  if (m_lecteur.getSymbole() == "<VARIABLE>") {
    Noeud *affect = affectation();
    testerEtAvancer(";");
    
    return affect;
  }
  else if (m_lecteur.getSymbole() == "si" )
   return instSi(); 
  else if (m_lecteur.getSymbole() == "tantque" )
   return instTantque(); 
  else if (m_lecteur.getSymbole() == "repeter" )
   return instRepeter(); 
  else if (m_lecteur.getSymbole() == "pour" )
   return instPour(); 
  else if (m_lecteur.getSymbole() == "ecrire" )
   return instEcrire();
  else if (m_lecteur.getSymbole() == "lire" )
   return instLire();
  else erreur("Instruction incorrecte");
      }
      catch (SyntaxeException e){
          cout << e.what();
          while (m_lecteur.getSymbole() != "si"  && m_lecteur.getSymbole() != "<VARIABLE>" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole() != "finproc" && m_lecteur.getSymbole() != "finsi" && m_lecteur.getSymbole() != "finpour" && m_lecteur.getSymbole() != "fintantque")
          {
              m_lecteur.avancer();
          }
          return NULL;
              
      }
    }

Noeud* Interpreteur::affectation() {
  // <affectation> ::= <variable> = <expression> 
  tester("<VARIABLE>");
  Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
  m_lecteur.avancer();
  testerEtAvancer("=");
  Noeud* exp = expression();             // On mémorise l'expression trouvée
  return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
  // <expression> ::= <facteur> { <opBinaire> <facteur> }
  //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
  Noeud* termeGauche = terme();
  while ( m_lecteur.getSymbole() == "+"  || m_lecteur.getSymbole() == "-") {
      Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
      m_lecteur.avancer();
      Noeud* termeDroit = facteur(); // On mémorise le symbole de l'opérateur
      termeGauche = new NoeudOperateurBinaire(operateur, termeGauche, termeDroit); // Et on construui un noeud opérateur binaire
  }
  return termeGauche; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::relation(){
    // <relation> ::== <expression> { <opRel> <expression> }
    Noeud* expressionGauche = expression();
    while (m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" || 
          m_lecteur.getSymbole() == "<"  || m_lecteur.getSymbole() == "<=" ||
          m_lecteur.getSymbole() == ">"  || m_lecteur.getSymbole() == ">="  ) {

    Symbole operateur = m_lecteur.getSymbole();
    m_lecteur.avancer();
    Noeud* expressionDroite = expression(); // On mémorise l'opérande droit
    expressionGauche= new NoeudOperateurBinaire(operateur,expressionGauche,expressionDroite);
}
    return expressionGauche;
}

Noeud* Interpreteur::relationEt(){
    Noeud* relationGauche = relation();
    while (m_lecteur.getSymbole() == "et"){
    
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* relationDroite = relation(); // On mémorise l'opérande droit
        relationGauche = new NoeudOperateurBinaire(operateur,relationGauche,relationDroite);
    }
    return relationGauche;
}

Noeud* Interpreteur::expBool(){
    Noeud* relationGauche = relationEt();
    while (m_lecteur.getSymbole() == "ou"){
    
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* relationDroite = relationEt(); // On mémorise l'opérande droit
        relationGauche = new NoeudOperateurBinaire(operateur,relationGauche,relationDroite);
    }
    
    return relationGauche;
}


Noeud* Interpreteur::terme() {
    // <terme> :: <facteur> { * <facteur> | / <facteur> }
    Noeud* facteurGauche = facteur();
    while (m_lecteur.getSymbole() == "*" ||m_lecteur.getSymbole() == "/"){
    
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* facteurDroit = facteur();
        facteurGauche = new NoeudOperateurBinaire(operateur,facteurGauche,facteurDroit);
    }
    return facteurGauche;
}

Noeud* Interpreteur::facteur() {
  // <facteur> ::= <entier> | <variable> | - <expBool> | non <expBool> | ( <expBool> )
  
  if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
    Noeud* expBool = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
  } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- expBool) par une soustraction binaire (0 - facteur)
    Noeud* expBool = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
  } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- expBool) par une soustraction binaire (0 - facteur)
    Noeud* expBool = new NoeudOperateurBinaire(Symbole("non"), expBool(), nullptr);
  } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
    m_lecteur.avancer();
    Noeud* expBool = expBool();
    testerEtAvancer(")");
  } else
    erreur("Facteur incorrect");
  return expBool;
}


Noeud* Interpreteur::instSi() {
  // <instSi> ::= si ( <expression> ) <seqInst> finsi
    try{
  testerEtAvancer("si");
  testerEtAvancer("(");
  Noeud* condition = expression(); // On mémorise la condition
  testerEtAvancer(")");
  Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
  testerEtAvancer("finsi");
  return new NoeudInstSi(condition, sequence); // Et on renvoie un noeud Instruction Si
    }
    catch (SyntaxeException e){
          cout << e.what();
          while (m_lecteur.getSymbole() != "si"  && m_lecteur.getSymbole() != "<VARIABLE>" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole() != "finproc" && m_lecteur.getSymbole() != "finsi" && m_lecteur.getSymbole() != "finpour" && m_lecteur.getSymbole() != "fintantque")
          {
              m_lecteur.avancer();
          }
          if (m_lecteur.getSymbole() == "finsi") m_lecteur.avancer();
          return NULL;
              
      }
    
}

Noeud* Interpreteur::instTantque() {
    try{
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition, sequence);
    }
    catch (SyntaxeException e){
          cout << e.what();
          while (m_lecteur.getSymbole() != "si"  && m_lecteur.getSymbole() != "<VARIABLE>" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole() != "finproc" && m_lecteur.getSymbole() != "finsi" && m_lecteur.getSymbole() != "finpour" && m_lecteur.getSymbole() != "fintantque")
          {
              m_lecteur.avancer();
          }
          if (m_lecteur.getSymbole() == "fintantque") m_lecteur.avancer();
          return NULL;
              
      }
}

Noeud* Interpreteur::instRepeter() {
    try {
    testerEtAvancer("repeter");
    Noeud* sequence = seqInst();
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    return new NoeudInstRepeter(sequence, condition);
    }
    catch (SyntaxeException e){
          cout << e.what();
          while (m_lecteur.getSymbole() != "si"  && m_lecteur.getSymbole() != "<VARIABLE>" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole() != "finproc" && m_lecteur.getSymbole() != "finsi" && m_lecteur.getSymbole() != "finpour" && m_lecteur.getSymbole() != "fintantque")
          {
              m_lecteur.avancer();
          }
          return NULL;
              
      }
}

Noeud* Interpreteur::instPour() {
    try{
    testerEtAvancer("pour");
    testerEtAvancer("(");
    Noeud* affectation1 = affectation();
    testerEtAvancer(";");
    Noeud* condition =  expression();
    testerEtAvancer(";");
    Noeud* affectation2 = affectation();
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finpour");
    return new NoeudInstPour(affectation1,condition,affectation2,sequence);
    }
    catch (SyntaxeException e){
          cout << e.what();
          while (m_lecteur.getSymbole() != "si"  && m_lecteur.getSymbole() != "<VARIABLE>" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole() != "finproc" && m_lecteur.getSymbole() != "finsi" && m_lecteur.getSymbole() != "finpour" && m_lecteur.getSymbole() != "fintantque")
          {
              m_lecteur.avancer();
          }
          if (m_lecteur.getSymbole() == "finpour") m_lecteur.avancer();
          return NULL;
              
      }
}

Noeud* Interpreteur::instEcrire() {
    try {
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    NoeudInstEcrire* seq;
    //Si c'est une chaine
    if (m_lecteur.getSymbole() == "<CHAINE>") {
        seq = new NoeudInstEcrire(m_table.chercheAjoute(m_lecteur.getSymbole()));
        m_lecteur.avancer();
      }
    //Sinon c'est une expression
    else  {
        seq = new NoeudInstEcrire(expression());
    }
    
    //Si plusieurs paramètres sont rentrés
    if(m_lecteur.getSymbole()==","){
        do {
            m_lecteur.avancer();
            if (m_lecteur.getSymbole() == "<CHAINE>") {
                seq->ajoute(m_table.chercheAjoute(m_lecteur.getSymbole()));
                m_lecteur.avancer();
            }
            else  {
                seq->ajoute(expression());
            }
        } while (m_lecteur.getSymbole() == ",");
    }
    testerEtAvancer(")");
    testerEtAvancer(";");
    return seq;
    }
    catch (SyntaxeException e){
          cout << e.what();
          while (m_lecteur.getSymbole() != "si"  && m_lecteur.getSymbole() != "<VARIABLE>" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole() != "finproc" && m_lecteur.getSymbole() != "finsi" && m_lecteur.getSymbole() != "finpour" && m_lecteur.getSymbole() != "fintantque")
          {
              m_lecteur.avancer();
          }
          return NULL;
              
      }
}

Noeud* Interpreteur::instLire() {
    try{
    testerEtAvancer("lire");
    testerEtAvancer("(");
    NoeudInstLire* sequence = new NoeudInstLire(m_table.chercheAjoute(m_lecteur.getSymbole()));
    m_lecteur.avancer();
    if(m_lecteur.getSymbole()==","){
        do {
        m_lecteur.avancer();
        sequence->ajoute(m_table.chercheAjoute(m_lecteur.getSymbole()));
        m_lecteur.avancer();
        } while (m_lecteur.getSymbole() == "," );
    }
    testerEtAvancer(")");
    testerEtAvancer(";");
    return sequence;
    }
    catch (SyntaxeException e){
          cout << e.what();
          while (m_lecteur.getSymbole() != "si"  && m_lecteur.getSymbole() != "<VARIABLE>" && m_lecteur.getSymbole() != "tantque" && m_lecteur.getSymbole() != "repeter" && m_lecteur.getSymbole() != "pour" && m_lecteur.getSymbole() != "ecrire" && m_lecteur.getSymbole() != "lire" && m_lecteur.getSymbole() != "finproc" && m_lecteur.getSymbole() != "finsi" && m_lecteur.getSymbole() != "finpour" && m_lecteur.getSymbole() != "fintantque")
          {
              m_lecteur.avancer();
          }
          return NULL;
              
      }
}

void Interpreteur::traduitEnJava(ostream & cout, unsigned int indentation) const {
   cout<<"import java.util.Scanner;"<<setw(4*indentation)<<"class Main {"<<endl<<setw(indentation)<<"static void main (String[] args){"<<endl;
   cout << "Scanner sc = new scanner(System.in);";
   //declaration des variables
   
   getArbre()->traduitEnJava(cout, indentation + 1);
   cout << setw(indentation)<<"}"<<endl<<setw(4*indentation)<<"}"<<endl;
    
}