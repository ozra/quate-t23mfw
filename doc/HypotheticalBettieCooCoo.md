/**
* Created:  2014-08-19
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/


namespace Bettie {

class Component {


    vector<Component*>      children;


};

class Doc : public Bettie::Component {

}

namespace Web {

class LoginForm : public Bettie::Component {

    init() {


        parent::init()

    }

    render() {

        parent::render()

    }

}





layout()

    doc(
        head(
            scripts,
            css,
            metas
        ),
        body(
            main_content
        )
    );
