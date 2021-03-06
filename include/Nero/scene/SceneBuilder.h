////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2019 SANOU A. K. Landry
/////////////////////////////////////////////////////////////
#ifndef SCENEBUILDER_H
#define SCENEBUILDER_H
///////////////////////////HEADERS//////////////////////////
//NERO
#include <Nero/resource/ResourceManager.h>
#include <Nero/object/MeshObject.h>
#include <Nero/object/SpriteObject.h>
#include <Nero/object/ButtonObject.h>
#include <Nero/object/AnimationObject.h>
#include <Nero/object/LayerObject.h>
#include <Nero/object/TextObject.h>
#include <Nero/object/UIObject.h>
#include <Nero/scene/MeshEditor.h>
#include <Nero/scene/PhysicObjectManager.h>
#include <Nero/scene/SceneUtil.h>
//SFGUI
#include <SFGUI/Canvas.hpp>
//SFML
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>
//STD
#include <unordered_map>
#include <memory>
////////////////////////////////////////////////////////////
namespace nero
{
    class SceneBuilder
    {
        public:
            typedef std::shared_ptr<SceneBuilder> Ptr;

        public:
                                            SceneBuilder(sfg::Canvas::Ptr renderCanvas, ResourceManager::Ptr resourceManager, SceneSetting& sceneSetting);

            //Main
            void                            handleEvent(const sf::Event& event);
            void                            update(const sf::Time& elapsedTime);
            void                            render();
            //Input
            void                            handleKeyboardInput(const sf::Keyboard::Key& key, const bool& isPressed);
            void                            handleMouseButtonsInput(const sf::Event::MouseButtonEvent& mouse, const bool& isPressed);
            void                            handleMouseMoveInput(const sf::Event::MouseMoveEvent& mouse);
            //Layer
            const LayerObject::Ptr          addLayer();
            void                            deleteLayer(int id);
            void                            moveLayerUp(int id);
            void                            moveLayerDown(int id);
            bool                            mergeLayerUp(int id);
            bool                            mergeLayerDown(int id);

            void                            selectLayer(int id);
            void                            updateLayerVisibility(int id);
            void                            updateLayerName(int id, sf::String name);

            const LayerObject::Tab&         getLayerTable() const;
            //object
            enum                            Position {Up, Down, Left, Right, Up_Left, Up_Right, Down_Left, Down_Right};

            bool                            addObject(Object::Type type, const sf::String& label, sf::Vector2f position = sf::Vector2f(0.f, 0.f));
            void                            copyObject(const Position& position);
            void                            deleteObject(Object::Ptr object);
            void                            setObjectName(const sf::String& name);
            void                            setObjectCategory(const sf::String& category);
            void                            moveObjectUp();
            void                            moveObjectDown();
            Object::Ptr                     getSelectedObject();
            //Sprite
            void                            updateLayerColor(const sf::Color& color);
            void                            updateAllLayerAlpha(int alpha);
            void                            updateSpriteColor(const sf::Color& color);
            void                            updateTextColor(const sf::Color& color);
            void                            updateOutlineTextColor(const sf::Color& color);
            sf::Color                       getLayerColor();
            //Mesh
            void                            setMeshType(const sf::String& label);
            void                            setMeshFixedRotation(bool tag);
            void                            setMeshSensor(bool tag);
            void                            setMeshAllowSleep(bool tag);
            void                            setMeshDensity(float density);
            void                            setMeshFriction(float friction);
            void                            setMeshRestitution(float restitution);
            void                            setMeshGravityScale(float gravityScale);
            //
            MeshEditor::Ptr                 getMeshEditor();
            //Scene
            void                            buildScene(Object::Ptr rootObject);
            void                            destroyAllPhysicObject(Object::Ptr mainObject);
            void                            updateLayerOrder();
            nlohmann::json                  saveScene();
            void                            loadScene(nlohmann::json scene);

            void                            setUpdateUI(std::function<void()>  fn);
            void                            setUpdateUndo(std::function<void()>  fn);
            void                            setUpdateLog(std::function<void(const std::string&, int)>  fn);
            void                            setUpdateLogIf(std::function<void(const std::string&, bool, int)>  fn);

            void                            setPhysicWorld(b2World* world);

            void                            buildUI(UIObject::Ptr rootObject);

            void                            setTextContent(const sf::String& content);
            void                            setTextFont(const sf::String& font);
            void                            setTextFontSize(float value);
            void                            setTextLetterSpacing(float value);
            void                            setTextLineSpacing(float value);
            void                            setTextOutlineThickness(float value);
            void                            setTextStyle(bool bold, bool italic, bool underLine, bool lineThrough);

            const sf::Color&                getCanvasColor() const;
            void                            setCanvasColor(const sf::Color& color);

            int                             getObjectCount();

        private:
            Object::Ptr                     findObject(Object::Ptr object, sf::Vector2f pos);

            void                            findObject(Object::Ptr object, int id, Object::Ptr& result);
            void                            findObject(Object::Ptr object, sf::String name, Object::Ptr& result);
            void                            findObject(Object::Ptr object, sf::Vector2f pos, Object::Ptr& result);

        public:
            Object::Ptr                     findObject(Object::Ptr object, sf::String name);
            Object::Ptr                     findObject(Object::Ptr object, int id);

        private:
            SpriteObject::Ptr               loadSprite(nlohmann::json& json);
            TextObject::Ptr                 loadText(nlohmann::json& json);
            MeshObject::Ptr                 loadMesh(nlohmann::json& json);
            AnimationObject::Ptr            loadAnimation(nlohmann::json& json);
            int                             getNewId();

        private:
            sfg::Canvas::Ptr                m_RenderCanvas;
            ResourceManager::Ptr            m_ResourceManager;
            sf::RectangleShape              m_SelectionRect;
            sf::Vector2f                    m_LastMousePosition;

            float                           m_PanningSpeed;
            float                           m_RotationSpeed;
            float                           m_ZoomingRatio;

            LayerObject::Tab                m_LayerTable;
            LayerObject::Ptr                m_SelectedLayer;
            int                             m_LayerCount;

            Object::Ptr                     m_SelectedObject;
            int                             m_ObjectCount;

            MeshEditor::Ptr                 m_MeshEditor;
            PhysicObjectManager             m_PhysicObjectManager;

            b2World*                        m_PhysicWorld;

            SceneSetting&                   m_SceneSetting;
            sf::Color                       m_CanvasColor;

            std::function<void()>                               m_UpdateUI;
            std::function<void()>                               m_UpdateUndo;
            std::function<void(const std::string&, int)>        m_UpdateLog;
            std::function<void(const std::string&, bool, int)>  m_UpdateLogIf;
    };
}
#endif // SCENEBUILDER_H
