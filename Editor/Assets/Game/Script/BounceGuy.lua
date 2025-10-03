function start()
    startPos = gameObject:getWorldPosition()

    resetTimer = 3
    currentTime = 3

    if gameObject.id == 149 then
        currentTime = 2
    end

     if gameObject.id == 153 then
        currentTime = 1
    end
end

function update(deltaTime)

    currentTime = currentTime + deltaTime

    if (currentTime >= resetTimer) then
        currentTime = 0
        gameObject:setLocalPosition(startPos)
		local physic = gameObject:getPhysicComponent()
		physic:setPosition(startPos)
        physic:resetVelocity()
        local force = Vector3f.new()
        force.x = 10
        force.y = 3
        physic:addImpulse(force)
    end  

end 