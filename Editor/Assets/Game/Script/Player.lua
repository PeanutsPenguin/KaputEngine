acceleration = 4
maxAirSpeed = 12
maxGroundSpeed = 5
errorMargin = 0.35

function start()
	isGrounded = false

	previousDirection = 0
	currentDirection = 0

	currentSpeed = 1
	errorTimer = 0

	fixedUpdate = 0.01666666666666667;
	fixedDt = fixedUpdate;

	local text = getUIObject("Timer")
    TimerText = text:castToUIText()
	timeSpent = 0

	local audio = gameObject:getAudioComponent()
	audio:play()
end

function move(deltaTime)
	local cam = gameObject:castToCamera()
	local physic = gameObject:getPhysicComponent()
	local front = cam.front

    local fw = 0
    local sd = 0

    if isKeyDown(Key.W) then
		currentDirection = 1
		 fw = fw + 1 
	end

    if isKeyDown(Key.S) then 
		currentDirection = 2
		fw = fw - 1 
	end

    if isKeyDown(Key.A) then
		currentDirection = 3
		 sd = sd - 1 
	end

    if isKeyDown(Key.D) then 
		currentDirection = 4
		sd = sd + 1 
	end

	local moveDirection = add(multiply(front, fw), multiply(cam.right, sd))

	if (fw ~= 0) or (sd ~= 0) then 
		if(currentDirection ~= previousDirection) and (isGrounded == false) then
			currentSpeed = 1 
		end

		moveDirection = moveDirection:normalize()
		currentSpeed = currentSpeed + acceleration * deltaTime
	else
		currentSpeed = 1
	end 

	previousDirection = currentDirection

	local moveVel = multiply(moveDirection, currentSpeed)
	moveVel.y = physic.linearVelocity.y	
	physic.linearVelocity = moveVel

	local noYVel = physic.linearVelocity
	noYVel.y = 0

	local velMag = noYVel:magnitudeSquared()

	if (isGrounded) then 
		if (errorTimer > errorMargin) then 
			if (velMag >= maxGroundSpeed * maxGroundSpeed) then
				local newVel = multiply(noYVel:normalize(), maxGroundSpeed)
				newVel.y = physic.linearVelocity.y
				physic.linearVelocity = newVel
				currentSpeed = maxGroundSpeed;
			end
		end 
	else 
		if (velMag >= maxAirSpeed * maxAirSpeed) then
			local newVel = multiply(noYVel:normalize(), maxAirSpeed)
			newVel.y = physic.linearVelocity.y
			physic.linearVelocity = newVel
			currentSpeed = maxAirSpeed
		end
	end
end

function look()
	local cam = gameObject:castToCamera()	

	local mouseDelta = getMouseDelta()

    local newPitch = cam.pitch + (mouseDelta.y * 0.1)
    local newYaw = cam.yaw + (mouseDelta.x * 0.1)

	if newPitch > 89 then newPitch = 89 end
    if newPitch < -89 then newPitch = -89 end

    cam.yaw = newYaw
    cam.pitch = newPitch
end 

function Jump()
	local physic = gameObject:getPhysicComponent()
	local force = Vector3f.new()
	force.y = 5

	if isKeyPressed(Key.Space) then 
		if(isGrounded)	then
			physic:addImpulse(force)
			isGrounded = false
			errorTimer = 0
		end
	end 
end 

function Teleportation()
	local physic = gameObject:getPhysicComponent()
	local tpPosition = Vector3f.new()
	tpPosition.y = 3 

	if isKeyPressed(Key.One) then
		tpPosition.x = 0
		tpPosition.z = -90
		gameObject:setLocalPosition(tpPosition)
		physic:setPosition(tpPosition)
	end

	if isKeyPressed(Key.Two) then
		tpPosition.x = 0
		tpPosition.z = -150
		gameObject:setLocalPosition(tpPosition)
		physic:setPosition(tpPosition)
	end

	if isKeyPressed(Key.Three) then
		tpPosition.x = 46
		tpPosition.z = -150
		gameObject:setLocalPosition(tpPosition)
		physic:setPosition(tpPosition)
	end

	if isKeyPressed(Key.Four) then
		tpPosition.x = 46
		tpPosition.z = -66
		gameObject:setLocalPosition(tpPosition)
		physic:setPosition(tpPosition)
	end
end

function update(deltaTime)
	fixedDt = fixedDt + deltaTime
	while (fixedDt >= fixedUpdate) do
		move(fixedUpdate)
		fixedDt = fixedDt - fixedUpdate
	end

	if (isGrounded) then 
		errorTimer = errorTimer + deltaTime
	end

	look()

	Jump()
	
	Teleportation()

	timeSpent = timeSpent + deltaTime

	TimerText.text = toString(timeSpent)

    -- if isKeyPressed(Key.Zero) then
	-- 	loadScene("Game/Scene/Padoru.kscene")
	-- end
end

function OnCollisionEnter(other)
	if(other:compareTag("Ground")) then 
		if (isKeyDown(Key.Space) == false) then
			isGrounded = true
			errorTimer = 0
		end
	end

	if(other:compareTag("Death")) then 
		local newPos = Vector3f.new()
		newPos.z = -15
		newPos.y = 5
		gameObject:setLocalPosition(newPos)
		local physic = gameObject:getPhysicComponent()
		physic:setPosition(newPos)
		timeSpent = 0
	end
end 

function OnCollision(other)
	if(other:compareTag("Ground")) then 
		if (isKeyDown(Key.Space) == false) then
			isGrounded = true
		end
	end
end 

function OnCollisionExit()  
	isGrounded = false
end 
