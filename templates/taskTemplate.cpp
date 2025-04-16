//
// Created by Jonas on 27/01/2023.
//

#include "meshcore/rendering/ApplicationWindow.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    ApplicationWindow window;
    window.show();
    RenderWidget* renderWidget = window.getRenderWidget();

    class TemplateSolution: AbstractSolution{
    public:
        /** Describe the solution data structure here **/
    };

    class: public AbstractTask {
    public:
        void run() override {
            /** Implement your algorithms, optimizations, ... here **/
//            notifyObserversSolution();
            notifyObserversStatus("Running");
            notifyObserversProgress(0.5);
//            notifyObserversUpdate();
        }
    } task;

    std::function<void(RenderWidget* renderWidget, std::shared_ptr<const AbstractSolution> solution)> onSolutionNotified = [&](RenderWidget* renderWidget, std::shared_ptr<const AbstractSolution> solution){
        // This is called every time a new solution should be rendered
        auto templateSolution = std::dynamic_pointer_cast<const TemplateSolution>(solution);

        /** Describe how a solution should be rendered **/
    };

    renderWidget->observeTask(&task, onSolutionNotified);
    task.start();
    int returnCode = QApplication::exec();
    task.stop();
    task.join();
    return returnCode;
}