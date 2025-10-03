function start()

    local Object = getUIObject("playbutton")
    button = Object:castToUIButton()

end

function update(deltaTime)
    if(button.isClicked) then
        setCursorHidden(true)
        loadScene("Game/Scene/newScene.kscene")
    end
end