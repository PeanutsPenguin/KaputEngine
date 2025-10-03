function start()
    startPos = gameObject:getWorldPosition()

end

function update(deltaTime)

    local currentPos = gameObject:getWorldPosition()

    if(currentPos.y < startPos.y - 10) then 
		gameObject:setLocalPosition(startPos)
		local physic = gameObject:getPhysicComponent()
		physic:setPosition(startPos)
        physic:resetVelocity()
        
        -- local force = Vector3f.new()
        -- force.y = -5
        -- physic:addImpulse(force)
    end 
end