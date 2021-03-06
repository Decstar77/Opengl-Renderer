#include "Editor.h"

namespace cm
{
	cm::Transform * Widget3D::GetTransform()
	{
		return &transform;
	}

	void Widget3D::SetTransform(const Transform &transform)
	{
		this->transform.position = transform.position;
		if (object_aligning)
		{
			this->transform.rotation = transform.rotation;
		}
		CalculateBoundingBoxes();
	}

	real32 Widget3D::GetSnappingAmount()
	{
		return snapping_amount;
	}

	void Widget3D::SetSnappingAmount(const real32 &deg)
	{
		snapping_amount = Abs(deg);
	}

	bool Widget3D::IsObjectAligning()
	{
		return object_aligning;
	}

	void Widget3D::ObjectAlign()
	{
		object_aligning = true;
	}

	void Widget3D::WorldAlign()
	{
		object_aligning = false;
		this->transform.rotation = Quatf();
	}

	const cm::GLMesh Widget3D::GetMeshForRender() const
	{
		return mesh;
	}


	TranslationWidget::TranslationWidget()
	{

	}

	TranslationWidget::~TranslationWidget()
	{

	}

	void TranslationWidget::Create(const GLMesh &mesh)
	{
		transform.position = Vec3f(0);
		transform.scale = Vec3f(0.04);
		this->mesh = mesh;
		CalculateBoundingBoxes();
	}

	bool TranslationWidget::Select(const Ray &camera_ray, const Transform &transform)
	{
		// @NOTE: Because of the way the bounding boxes are created (by messing the extents not rotating)
		//		: They all share the same basis vectors
		is_selected = false;

		CollisionInfo colinfo;
		real32 closest_box = REAL_MAX;

		// @NOTE: Check x-axis
		bool x_axis= x_bounding_volume.CheckCollision(camera_ray, &colinfo);
		if (x_axis && closest_box > colinfo.dist)
		{			
			real32 df = Dot(camera_ray.direction, x_bounding_volume.basis.forward);
			real32 dr = Dot(camera_ray.direction, x_bounding_volume.basis.right);
			real32 du = Dot(camera_ray.direction, x_bounding_volume.basis.upward);
			
			// @NOTE: Pick the the smallest dot and create a plane from the smallest basis
			//		: execpt the basis that well slide on in this case the right vector
			if (df < dr)
			{
				if (du < df)
				{
					translation_plane = Plane(transform.position, x_bounding_volume.basis.upward);
				}
				else
				{
					translation_plane = Plane(transform.position, x_bounding_volume.basis.forward);
				}
			}
			else
			{
				if (du < dr)
				{
					translation_plane = Plane(transform.position, x_bounding_volume.basis.upward);
				}
				else
				{
					translation_plane = Plane(transform.position, x_bounding_volume.basis.forward);
				}
			}			

			this->transform.position = transform.position;
			if (object_aligning) {
				this->transform.rotation = transform.rotation;
			}
						
			translation_plane.CheckCollision(camera_ray, &colinfo);

			Vec3f p0 = translation_plane.origin;
			Vec3f p1 = camera_ray.Travel(colinfo.dist);

			// @NOTE: Offset the origin to stop snapping
			translation_plane.origin += p1 - p0;

			// @NOTE: Set mode
			transformation_mode = TransformationMode::x_axis;
			
			// @NOTE: Set dist to check against
			closest_box = colinfo.dist;
			is_selected = true;
		}

		// @NOTE: Check y-axis
		bool y_axis = y_bounding_volume.CheckCollision(camera_ray, &colinfo);
		if (y_axis && closest_box > colinfo.dist)
		{
			// @NOTE: Create a plane where the origin is the intersection and normal point towards the camera
			real32 df = Dot(camera_ray.direction, y_bounding_volume.basis.forward);
			real32 dr = Dot(camera_ray.direction, y_bounding_volume.basis.right);
			real32 du = Dot(camera_ray.direction, y_bounding_volume.basis.upward);

			// @NOTE: Pick the the smallest dot and create a plane from the smallest basis
			//		: execpt the basis that well slide on in this case the up vector
			if (df < dr)
			{
				if (du < df)
				{
					translation_plane = Plane(transform.position, y_bounding_volume.basis.forward);
				}
				else
				{
					translation_plane = Plane(transform.position, y_bounding_volume.basis.forward);
				}
			}
			else
			{
				if (du < dr)
				{
					translation_plane = Plane(transform.position, y_bounding_volume.basis.forward);
				}
				else
				{
					translation_plane = Plane(transform.position, y_bounding_volume.basis.right);
				}
			}

			this->transform.position = transform.position;
			if (object_aligning) {
				this->transform.rotation = transform.rotation;
			}

			translation_plane.CheckCollision(camera_ray, &colinfo);

			Vec3f p0 = translation_plane.origin;
			Vec3f p1 = camera_ray.Travel(colinfo.dist);

			// @NOTE: Offset the origin to stop snapping
			translation_plane.origin += p1 - p0;

			// @NOTE: Set mode
			transformation_mode = TransformationMode::y_axis;

			// @NOTE: Set dist to check against
			closest_box = colinfo.dist;
			is_selected = true;
		}

		// @NOTE: Check z-axis
		bool z_axis = z_bounding_volume.CheckCollision(camera_ray, &colinfo);
		if (z_axis && closest_box > colinfo.dist)
		{
			// @NOTE: Create a plane where the origin is the intersection and normal point towards the camera
			real32 df = Dot(camera_ray.direction, z_bounding_volume.basis.forward);
			real32 dr = Dot(camera_ray.direction, z_bounding_volume.basis.right);
			real32 du = Dot(camera_ray.direction, z_bounding_volume.basis.upward);

			// @NOTE: Pick the the smallest dot and create a plane from the smallest basis
			//		: execpt the basis that well slide on in this case the right vector
			if (du < df)
			{
				if (du < dr)
				{
					translation_plane = Plane(transform.position, z_bounding_volume.basis.upward);
				}
				else
				{
					translation_plane = Plane(transform.position, z_bounding_volume.basis.right);
				}
			}
			else
			{
				if (dr < df)
				{
					translation_plane = Plane(transform.position, z_bounding_volume.basis.right);
				}
				else
				{
					translation_plane = Plane(transform.position, z_bounding_volume.basis.upward);
				}
			}

			this->transform.position = transform.position;
			if (object_aligning) {
				this->transform.rotation = transform.rotation;
			}
			

			translation_plane.CheckCollision(camera_ray, &colinfo);

			Vec3f p0 = translation_plane.origin;
			Vec3f p1 = camera_ray.Travel(colinfo.dist);

			// @NOTE: Offset the origin to stop snapping
			translation_plane.origin += p1 - p0;

			// @NOTE: Set mode
			transformation_mode = TransformationMode::z_axis;

			// @NOTE: Set dist to check against
			closest_box = colinfo.dist;
			is_selected = true;
		}

		return is_selected;
	}

	bool TranslationWidget::Update(const Ray &camera_ray, Transform *transform)
	{
		if (is_selected) 
		{
			Assert(transform);
			CollisionInfo colinfo;
			translation_plane.CheckCollision(camera_ray, &colinfo);

			Vec3f p0 = translation_plane.origin;
			Vec3f p1 = camera_ray.Travel(colinfo.dist);
			
			Vec3f delta = p1 - p0;
			
			switch (transformation_mode)
			{
			case cm::TransformationMode::none: {
				LOG(" WARNING: Translating nothing ??");
				break;
			}
			case cm::TransformationMode::x_axis: {
				Vec3f dir = Normalize( x_bounding_volume.basis.right );
				real32 dist = Dot(delta, x_bounding_volume.basis.right);

				if (Abs(dist) < snapping_amount)
				{
					break;
				}
				else if (Abs(dist) > snapping_amount && snapping_amount > 0)
				{
					dist = snapping_amount * Sign(dist);
				}

				this->transform.position += dir * dist ;
				translation_plane.origin = p1;
				break;
			}
			case cm::TransformationMode::y_axis: {
				Vec3f dir = Normalize(y_bounding_volume.basis.upward);
				real32 dist = Dot(delta, y_bounding_volume.basis.upward);
				
				if (Abs(dist) < snapping_amount)
				{
					break;
				}
				else if (Abs(dist) > snapping_amount && snapping_amount > 0)
				{
					dist = snapping_amount * Sign(dist);
				}

				this->transform.position += dir * dist;
				translation_plane.origin = p1;

				break;
			}
			case cm::TransformationMode::z_axis: {
				Vec3f dir = Normalize(z_bounding_volume.basis.forward);
				real32 dist = Dot(delta, z_bounding_volume.basis.forward);

				if (Abs(dist) < snapping_amount)
				{
					break;
				}
				else if (Abs(dist) > snapping_amount && snapping_amount > 0)
				{
					dist = snapping_amount * Sign(dist);
				}

				this->transform.position += dir * dist;
				translation_plane.origin = p1;
				break;
			}
			default: {
				LOG(" WARNING: Translating nothing ??");
				break;
			}
			}
			transform->position = this->transform.position;
			CalculateBoundingBoxes();
		}
		return is_selected;
	}

	void TranslationWidget::CalculateBoundingBoxes()
	{
		// @NOTE: We take the conjugate because of model's blender coords ??
		Mat3f rotation = QuatToMat3(Conjugate(transform.rotation));
		Basis local_basis(rotation);

		Vec3f x_origin = Vec3f(0.36, 0, 0) * local_basis.mat;
		Vec3f y_origin = Vec3f(0, 0.36, 0) * local_basis.mat;
		Vec3f z_origin = Vec3f(0, 0, 0.36) * local_basis.mat;

		x_bounding_volume = OBB(transform.position + x_origin, Vec3f(0.30, 0.05, 0.05), local_basis);
		y_bounding_volume = OBB(transform.position + y_origin, Vec3f(0.05, 0.30, 0.05), local_basis);
		z_bounding_volume = OBB(transform.position + z_origin, Vec3f(0.05, 0.05, 0.30), local_basis);
	}
	
	RotationWidget::RotationWidget()
	{

	}

	RotationWidget::~RotationWidget()
	{

	}

	void RotationWidget::Create(const GLMesh &mesh)
	{
		transform.position = Vec3f(0);
		transform.scale = Vec3f(0.04);
		this->mesh = mesh;
		CalculateBoundingBoxes();
	}

	bool RotationWidget::Select(const Ray &camera_ray, const Transform &transform)
	{
		is_selected = false;
		CollisionInfo colinfo;
		real32 closest_box = REAL_MAX;
		
		for (int32 i = 0; i < 4; i++)
		{
			x_bounding_volumes[i].CheckCollision(camera_ray, &colinfo);
			if (colinfo.hit && closest_box > colinfo.dist)
			{
				transformation_mode = TransformationMode::x_axis;
				transform_plane = Plane(x_bounding_volumes[i].origin, camera_ray.direction);

				this->transform.position = transform.position;
				if (object_aligning)
				{
					this->transform.rotation = transform.rotation;
				}

				is_selected = true;
				closest_box = colinfo.dist;
			}		
		}
		
		for (int32 i = 0; i < 4; i++)
		{
			y_bounding_volumes[i].CheckCollision(camera_ray, &colinfo);
			if (colinfo.hit && closest_box > colinfo.dist)
			{
				transformation_mode = TransformationMode::y_axis;
				transform_plane = Plane(y_bounding_volumes[i].origin, camera_ray.direction);
				
				this->transform.position = transform.position;
				if (object_aligning)
				{
					this->transform.rotation = transform.rotation;
				}
				

				is_selected = true;
				closest_box = colinfo.dist;
			}
		}

		for (int32 i = 0; i < 4; i++)
		{
			z_bounding_volumes[i].CheckCollision(camera_ray, &colinfo);
			if (colinfo.hit && closest_box > colinfo.dist)
			{
				transformation_mode = TransformationMode::z_axis;
				transform_plane = Plane(z_bounding_volumes[i].origin, camera_ray.direction);
				
				this->transform.position = transform.position;
				if (object_aligning)
				{
					this->transform.rotation = transform.rotation;
				}

				is_selected = true;
				closest_box = colinfo.dist;
			}
		}
		return is_selected;
	}

	bool RotationWidget::Update(const Ray &camera_ray, Transform *transform)
	{
		if (is_selected)
		{
			CollisionInfo colinfo;
			transform_plane.CheckCollision(camera_ray, &colinfo);

			Vec3f p0 = transform_plane.origin;
			Vec3f p1 = camera_ray.Travel(colinfo.dist);

			Vec3f d0 = Normalize(p0 - this->transform.position);
			Vec3f d1 = Normalize(p1 - this->transform.position);
			
			switch (transformation_mode)
			{
			case cm::TransformationMode::none: {
				LOG(" WARNING: Rotating nothing ??");
				break;
			}
			case cm::TransformationMode::x_axis: {
				// @NOTE: Get the axis of rotation
				Basis b = x_bounding_volumes[0].basis;

				// @NOTE: Normalize for safety
				Vec3f ncr = Normalize(camera_ray.direction);
				Vec3f nbr = Normalize(b.right);

				// @NOTE: Get the angle theta from d0 to d1. The clamp is for floating point errors 
				real32 theta = Clamp(Dot(d0, d1), -1.0f, 1.0f);

				// @NOTE: The first dot is determining which way the user is dragging the widget
				//		: The second is determining if the user is facing away or two the right basis vector
				//		: of the widget. This is to determine the correct rotation direction
				int32 dir = Sign( Dot(Cross(d0, d1), ncr) ) * Sign( Dot(ncr, nbr) ) * -1;

				// @NOTE: Conversions
				theta = RadToDeg(acos(theta) * dir);

				// @NOTE: Snapping
				if (Abs(theta) < snapping_amount)
				{
					break;
				}
				else if (Abs(theta) > snapping_amount && snapping_amount > 0.0f)
				{
					theta = snapping_amount * dir;
				}
				
				// @NOTE: Apply the rotation
				Quatf r = Rotate(transform->rotation, theta, nbr);
				transform->rotation = r * transform->rotation;

				// @NOTE: Put the origin where the ray intersected the plane
				transform_plane.origin = p1;
				break; 
			}
			case cm::TransformationMode::y_axis: {
				// @NOTE: Get the axis of rotation
				Basis b = y_bounding_volumes[0].basis;

				// @NOTE: Normalize for safety
				Vec3f ncr = Normalize(camera_ray.direction);
				Vec3f nbu = Normalize(b.upward);

				// @NOTE: Get the angle theta from d0 to d1. The clamp is for floating point errors 
				real32 theta = Clamp(Dot(d0, d1), -1.0f, 1.0f);

				// @NOTE: The first dot is determining which way the user is dragging the widget
				//		: The second is determining if the user is facing away or two the right basis vector
				//		: of the widget. This is to determine the correct rotation direction
				int32 dir = Sign(Dot(Cross(d0, d1), ncr)) * Sign(Dot(ncr, nbu)) * -1;

				// @NOTE: Conversions
				theta = RadToDeg(acos(theta) * dir);

				// @NOTE: Snapping
				if (Abs(theta) < snapping_amount)
				{
					break;
				}
				else if (Abs(theta) > snapping_amount && snapping_amount > 0.0f)
				{
					theta = snapping_amount * dir;
				}

				// @NOTE: Apply the rotation
				Quatf r = Rotate(transform->rotation, theta, nbu);
				transform->rotation = r * transform->rotation;

				// @NOTE: Put the origin where the ray intersected the plane
				transform_plane.origin = p1;
				break;
			}
			case cm::TransformationMode::z_axis: {
				Basis b = z_bounding_volumes[0].basis;

				// @NOTE: Normalize for safety
				Vec3f ncr = Normalize(camera_ray.direction);
				Vec3f nbf = Normalize(b.forward);

				// @NOTE: Get the angle theta from d0 to d1. The clamp is for floating point errors 
				real32 theta = Clamp(Dot(d0, d1), -1.0f, 1.0f);

				// @NOTE: The first dot is determining which way the user is dragging the widget
				//		: The second is determining if the user is facing away or two the right basis vector
				//		: of the widget. This is to determine the correct rotation direction
				int32 dir = Sign(Dot(Cross(d0, d1), ncr)) * Sign(Dot(ncr, nbf)) * -1;

				// @NOTE: Conversions
				theta = RadToDeg(acos(theta) * dir);

				// @NOTE: Snapping
				if (Abs(theta) < snapping_amount)
				{
					break;
				}
				else if (Abs(theta) > snapping_amount && snapping_amount > 0.0f)
				{
					theta = snapping_amount * dir;
				}

				// @NOTE: Apply the rotation
				Quatf r = Rotate(transform->rotation, theta, nbf);
				transform->rotation = r * transform->rotation;
			

				// @NOTE: Put the origin where the ray intersected the plane
				transform_plane.origin = p1;
				break;
			}
			default: {
				LOG(" WARNING: Rotating nothing ??");
				break;
			}
			}

			if (object_aligning)
			{
				this->transform.rotation = transform->rotation;
			}
			
			CalculateBoundingBoxes();
		}
		return is_selected;
	}

	void RotationWidget::CalculateBoundingBoxes()
	{		
		Mat3f rotation = QuatToMat3(Conjugate(transform.rotation));
		Basis local_basis(rotation);

		// @NOTE: x-axis 
		Vec3f x_origin01 = Vec3f(0, 0.4, 0.4)	  * local_basis.mat;
		Vec3f x_origin02 = Vec3f(0, -0.4, -0.4) * local_basis.mat;
		Vec3f x_origin03 = Vec3f(0, 0.4, -0.4)  * local_basis.mat;
		Vec3f x_origin04 = Vec3f(0, -0.4, 0.4)  * local_basis.mat;

		x_bounding_volumes[0] = OBB(transform.position + x_origin01, Vec3f(0.05, 0.2, 0.05), Rotate(local_basis.mat, -45.0f, Vec3f(1, 0, 0)));
		x_bounding_volumes[1] = OBB(transform.position + x_origin02, Vec3f(0.05, 0.2, 0.05), Rotate(local_basis.mat, -45.0f, Vec3f(1, 0, 0)));
		x_bounding_volumes[2] = OBB(transform.position + x_origin03, Vec3f(0.05, 0.2, 0.05), Rotate(local_basis.mat, 45.0f, Vec3f(1, 0, 0)));
		x_bounding_volumes[3] = OBB(transform.position + x_origin04, Vec3f(0.05, 0.2, 0.05), Rotate(local_basis.mat, 45.0f, Vec3f(1, 0, 0)));

		// @NOTE: y-axis 
		Vec3f y_origin01 = Vec3f(0.4, 0, 0.4)	  * local_basis.mat;
		Vec3f y_origin02 = Vec3f(-0.4, 0, -0.4) * local_basis.mat;
		Vec3f y_origin03 = Vec3f(0.4, 0, -0.4)  * local_basis.mat;
		Vec3f y_origin04 = Vec3f(-0.4, 0, 0.4)  * local_basis.mat;

		y_bounding_volumes[0] = OBB(transform.position + y_origin01, Vec3f(0.2, 0.05, 0.05), Rotate(local_basis.mat, 45.0f, Vec3f(0, 1, 0)));
		y_bounding_volumes[1] = OBB(transform.position + y_origin02, Vec3f(0.2, 0.05, 0.05), Rotate(local_basis.mat, 45.0f, Vec3f(0, 1, 0)));
		y_bounding_volumes[2] = OBB(transform.position + y_origin03, Vec3f(0.2, 0.05, 0.05), Rotate(local_basis.mat, -45.0f, Vec3f(0, 1, 0)));
		y_bounding_volumes[3] = OBB(transform.position + y_origin04, Vec3f(0.2, 0.05, 0.05), Rotate(local_basis.mat, -45.0f, Vec3f(0, 1, 0)));

		// @NOTE: z-axis, 0.6 because of pythag.
		Vec3f z_origin01 = Vec3f(0.6, 0, 0)	* local_basis.mat;
		Vec3f z_origin02 = Vec3f(0, 0.6, 0)	* local_basis.mat;
		Vec3f z_origin03 = Vec3f(0, -0.6, 0)	* local_basis.mat;
		Vec3f z_origin04 = Vec3f(-0.6, 0, 0)	* local_basis.mat;

		z_bounding_volumes[0] = OBB(transform.position + z_origin01, Vec3f(0.05, 0.2, 0.05), local_basis);
		z_bounding_volumes[1] = OBB(transform.position + z_origin02, Vec3f(0.2, 0.05, 0.05), local_basis);
		z_bounding_volumes[2] = OBB(transform.position + z_origin03, Vec3f(0.2, 0.05, 0.05), local_basis);
		z_bounding_volumes[3] = OBB(transform.position + z_origin04, Vec3f(0.05, 0.2, 0.05), local_basis);
	}

	ScalingWidget::ScalingWidget()
	{

	}

	ScalingWidget::~ScalingWidget()
	{

	}

	void ScalingWidget::Create(const GLMesh &mesh)
	{
		transform.position = Vec3f(0);
		transform.scale = Vec3f(0.04);
		this->mesh = mesh;
		CalculateBoundingBoxes();
	}

	bool ScalingWidget::Select(const Ray &camera_ray, const Transform &transform)
	{
		// @NOTE: Because of the way the bounding boxes are created (by messing the extents not rotating)
		//		: They all share the same basis vectors
		//		: This is the exact same code from the translation widget
		is_selected = false;

		CollisionInfo colinfo;
		real32 closest_box = REAL_MAX;

		// @NOTE: Check x-axis
		bool x_axis = x_bounding_volume.CheckCollision(camera_ray, &colinfo);
		if (x_axis && closest_box > colinfo.dist)
		{
			real32 df = Dot(camera_ray.direction, x_bounding_volume.basis.forward);
			real32 dr = Dot(camera_ray.direction, x_bounding_volume.basis.right);
			real32 du = Dot(camera_ray.direction, x_bounding_volume.basis.upward);

			// @NOTE: Pick the the smallest dot and create a plane from the smallest basis
			//		: execpt the basis that well slide on in this case the right vector
			if (df < dr)
			{
				if (du < df)
				{
					transformation_plane = Plane(transform.position, x_bounding_volume.basis.upward);
				}
				else
				{
					transformation_plane = Plane(transform.position, x_bounding_volume.basis.forward);
				}
			}
			else
			{
				if (du < dr)
				{
					transformation_plane = Plane(transform.position, x_bounding_volume.basis.upward);
				}
				else
				{
					transformation_plane = Plane(transform.position, x_bounding_volume.basis.forward);
				}
			}

			this->transform.position = transform.position;
			this->transform.rotation = transform.rotation;

			transformation_plane.CheckCollision(camera_ray, &colinfo);

			Vec3f p0 = transformation_plane.origin;
			Vec3f p1 = camera_ray.Travel(colinfo.dist);

			// @NOTE: Offset the origin to stop snapping
			transformation_plane.origin += p1 - p0;

			// @NOTE: Set mode
			transformation_mode = TransformationMode::x_axis;

			// @NOTE: Set dist to check against
			closest_box = colinfo.dist;
			is_selected = true;
		}

		// @NOTE: Check y-axis
		bool y_axis = y_bounding_volume.CheckCollision(camera_ray, &colinfo);
		if (y_axis && closest_box > colinfo.dist)
		{
			// @NOTE: Create a plane where the origin is the intersection and normal point towards the camera
			real32 df = Dot(camera_ray.direction, y_bounding_volume.basis.forward);
			real32 dr = Dot(camera_ray.direction, y_bounding_volume.basis.right);
			real32 du = Dot(camera_ray.direction, y_bounding_volume.basis.upward);

			// @NOTE: Pick the the smallest dot and create a plane from the smallest basis
			//		: execpt the basis that well slide on in this case the up vector
			if (df < dr)
			{
				if (du < df)
				{
					transformation_plane = Plane(transform.position, y_bounding_volume.basis.forward);
				}
				else
				{
					transformation_plane = Plane(transform.position, y_bounding_volume.basis.forward);
				}
			}
			else
			{
				if (du < dr)
				{
					transformation_plane = Plane(transform.position, y_bounding_volume.basis.forward);
				}
				else
				{
					transformation_plane = Plane(transform.position, y_bounding_volume.basis.right);
				}
			}

			this->transform.position = transform.position;
			this->transform.rotation = transform.rotation;

			transformation_plane.CheckCollision(camera_ray, &colinfo);

			Vec3f p0 = transformation_plane.origin;
			Vec3f p1 = camera_ray.Travel(colinfo.dist);

			// @NOTE: Offset the origin to stop snapping
			transformation_plane.origin += p1 - p0;

			// @NOTE: Set mode
			transformation_mode = TransformationMode::y_axis;

			// @NOTE: Set dist to check against
			closest_box = colinfo.dist;
			is_selected = true;
		}

		// @NOTE: Check z-axis
		bool z_axis = z_bounding_volume.CheckCollision(camera_ray, &colinfo);
		if (z_axis && closest_box > colinfo.dist)
		{
			// @NOTE: Create a plane where the origin is the intersection and normal point towards the camera
			real32 df = Dot(camera_ray.direction, z_bounding_volume.basis.forward);
			real32 dr = Dot(camera_ray.direction, z_bounding_volume.basis.right);
			real32 du = Dot(camera_ray.direction, z_bounding_volume.basis.upward);

			// @NOTE: Pick the the smallest dot and create a plane from the smallest basis
			//		: execpt the basis that well slide on in this case the right vector
			if (du < df)
			{
				if (du < dr)
				{
					transformation_plane = Plane(transform.position, z_bounding_volume.basis.upward);
				}
				else
				{
					transformation_plane = Plane(transform.position, z_bounding_volume.basis.right);
				}
			}
			else
			{
				if (dr < df)
				{
					transformation_plane = Plane(transform.position, z_bounding_volume.basis.right);
				}
				else
				{
					transformation_plane = Plane(transform.position, z_bounding_volume.basis.upward);
				}
			}

			this->transform.position = transform.position;
			this->transform.rotation = transform.rotation;


			transformation_plane.CheckCollision(camera_ray, &colinfo);

			Vec3f p0 = transformation_plane.origin;
			Vec3f p1 = camera_ray.Travel(colinfo.dist);

			// @NOTE: Offset the origin to stop snapping
			transformation_plane.origin += p1 - p0;

			// @NOTE: Set mode
			transformation_mode = TransformationMode::z_axis;

			// @NOTE: Set dist to check against
			closest_box = colinfo.dist;
			is_selected = true;
		}

		return is_selected;
	}

	bool ScalingWidget::Update(const Ray &camera_ray, Transform *transform)
	{
		if (is_selected)
		{
			Assert(transform);
			CollisionInfo colinfo;
			transformation_plane.CheckCollision(camera_ray, &colinfo);

			Vec3f p0 = transformation_plane.origin;
			Vec3f p1 = camera_ray.Travel(colinfo.dist);

			Vec3f delta = p1 - p0;

			switch (transformation_mode)
			{
			case cm::TransformationMode::none: {
				LOG(" WARNING: Translating nothing ??");
				break;
			}
			case cm::TransformationMode::x_axis: {
				Vec3f dir = Normalize(x_bounding_volume.basis.right);
				real32 dist = Dot(delta, x_bounding_volume.basis.right);
				transform->scale.x += dist;					
				transformation_plane.origin = p1;
				break;
			}
			case cm::TransformationMode::y_axis: {
				Vec3f dir = Normalize(y_bounding_volume.basis.upward);
				real32 dist = Dot(delta, y_bounding_volume.basis.upward);
				transform->scale.y += dist;
				transformation_plane.origin = p1;
				break;
			}
			case cm::TransformationMode::z_axis: {
				Vec3f dir = Normalize(z_bounding_volume.basis.forward);
				real32 dist = Dot(delta, z_bounding_volume.basis.forward);
				transform->scale.z += dist;
				transformation_plane.origin = p1;
				break;
			}
			default: {
				LOG(" WARNING: Translating nothing ??");
				break;
			}
			}
			transform->position = this->transform.position;
			CalculateBoundingBoxes();
		}
		return is_selected;
	}

	void ScalingWidget::WorldAlign()
	{
		object_aligning = true;
		Debug::LogWarning("We do not support abritary scaling");
	}

	void ScalingWidget::CalculateBoundingBoxes()
	{
		Mat3f rotation = QuatToMat3(Conjugate(transform.rotation));
		Basis local_basis(rotation);

		Vec3f x_origin = Vec3f(0.36, 0, 0) * local_basis.mat;
		Vec3f y_origin = Vec3f(0, 0.36, 0) * local_basis.mat;
		Vec3f z_origin = Vec3f(0, 0, 0.36) * local_basis.mat;

		x_bounding_volume = OBB(transform.position + x_origin, Vec3f(0.30, 0.05, 0.05), local_basis);
		y_bounding_volume = OBB(transform.position + y_origin, Vec3f(0.05, 0.30, 0.05), local_basis);
		z_bounding_volume = OBB(transform.position + z_origin, Vec3f(0.05, 0.05, 0.30), local_basis);
	}

	//---------------------------------------------------------------
	//---------------------------------------------------------------
	//---------------------------------------------------------------
	//---------------------------------------------------------------
	//---------------------------------------------------------------
	//---------------------------------------------------------------

	void InitializeEditor(GLFWwindow *window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		ImGui::StyleColorsClassic();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}


		bool window_res = ImGui_ImplGlfw_InitForOpenGL(window, true);
		bool opengl_res = ImGui_ImplOpenGL3_Init("#version 410");

		Assert(window_res); // @REASON: Failed init for glfw openGl
		Assert(opengl_res); // @REASON: Failed init for openGl3 
	}

	void EditorBeginFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void EditorEndFrame()
	{
		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		io.DisplaySize = ImVec2(1280, 720);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void FreeEditor()
	{
		ImGui::DestroyContext();
	}


	void EditorConsole::LogInfo(const String &msg)
	{
		log_info << msg << '\n';
	}

	void EditorConsole::LogInfo(const bool msg)
	{
		log_info << msg << '\n';
	}

	static void  Strtrim(char* str) { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

	void EditorConsole::LogWarning(const String &msg)
	{
		log_warnings << "WARNING: " << msg << '\n';
	}

	void EditorConsole::LogWarning(const bool msg)
	{
		log_warnings << "WARNING: " << msg << '\n';
	}

	void EditorConsole::UpdateAndDraw()
	{
		ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
		ImGui::Begin(title.c_str());

		if (ImGui::SmallButton("Clear")) { ClearLogger(); } ImGui::SameLine();
		bool copy_to_clipboard = ImGui::SmallButton("Copy"); ImGui::SameLine();
		scroll_to_bottom = ImGui::SmallButton("Scroll to bottom"); ImGui::SameLine();
		ImGui::Checkbox("Auto scroll", &auto_scroll);


		ImGui::Separator();
		const real32 footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text

		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText

		if (copy_to_clipboard)
		{
			ImGui::LogToClipboard();
		}

		log_info << Debug::log_info_stream.str();
		log_warnings << Debug::log_warnings_stream.str();
		
		Debug::ClearLogger();
		String info = log_info.str();
		String warnings = log_warnings.str();

		if (warnings.length() > 0)
		{
			ImGui::SetWindowFocus();
		}

		if (Filter.PassFilter(info.c_str()))
		{
			ImGui::TextUnformatted(info.c_str());
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85, 0.8, 0, 1));
			ImGui::TextUnformatted(warnings.c_str());
		}	

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

		if (copy_to_clipboard)
		{
			ImGui::LogFinish();
		}

		if (auto_scroll || scroll_to_bottom)
		{
			ImGui::SetScrollHereY(1.0f);
		}


		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::EndChild();

		ImGui::Separator();

		//// @NOTE: Commands
		if (ImGui::InputText("Input", input_buffer, IM_ARRAYSIZE(input_buffer), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			String cmd = input_buffer;
			ExeCommand(cmd);
			strcpy_s(input_buffer, "");
		}

		ImGui::End();

	}

	void EditorConsole::ClearLogger()
	{
		// @NOTE: C++ man.... you'd think the .clear() would do...
		//		: Yup this is how you clear a stringstream
		history.str(String());
		log_info.str(String());
		log_warnings.str(String());
		commands.str(String());
	}


	void EditorConsole::ExeCommand(const String &cmd)
	{
		if (cmd == "help")
		{
			LogInfo("Middle mouse to free look");
		}
	}

	void EditorRenderSettingsWindow::UpdateAndDraw()
	{
		Assert(render_settings);
		ImGui::Begin("Render Settings");

		if (ImGui::CollapsingHeader("Render Passes"))
		{
			if (ImGui::TreeNode("Shadow Pass"))
			{
				ImGui::Text("On the todo: When I'm smart enough");
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("SSR Pass"))
			{
				ImGui::Checkbox("SSR", &render_settings->ssr);
				ImGui::DragFloat("Step length", &render_settings->ssr_ray_step_length, 0.001, 0.0, 2.0);
				ImGui::DragFloat("Max dist", &render_settings->ssr_ray_max_distance, 1.0, 0.0, 1000.0);
				ImGui::DragFloat("Hits required", &render_settings->ssr_ray_hit_tollerance, 1.0, 0.0, 10.0);

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("SSAO Pass"))
			{
				render_settings->ssao_changed = render_settings->ssao_changed | ImGui::Checkbox("SSAO", &render_settings->ssao);
				render_settings->ssao_changed = render_settings->ssao_changed | ImGui::SliderFloat("Radius", &render_settings->ssao_radius, 0.1, 5.0);
				render_settings->ssao_changed = render_settings->ssao_changed | ImGui::SliderFloat("Strength", &render_settings->ssao_strength, 0.1, 5.0);
				render_settings->ssao_changed = render_settings->ssao_changed | ImGui::SliderFloat("Bias", &render_settings->ssao_bias, 0.1, 0.001);
				render_settings->ssao_changed = render_settings->ssao_changed | ImGui::SliderInt("Kernel Size", &render_settings->ssao_kernel_size, 1, 64);
				ImGui::TreePop();
			}
		}

		if (ImGui::CollapsingHeader("Post Processing"))
		{

			if (ImGui::TreeNode("Tone Mapping"))
			{
				ImGui::DragFloat("Exposure", &render_settings->post_processing_exposure, 0.1f);

				render_settings->tonemapping = ImGui::RadioButton("RienHard", render_settings->tonemapping == RenderSettings::Reinhard) ? RenderSettings::Reinhard : render_settings->tonemapping;
				render_settings->tonemapping = ImGui::RadioButton("Uncharted", render_settings->tonemapping == RenderSettings::Uncharted) ? RenderSettings::Uncharted : render_settings->tonemapping;
				render_settings->tonemapping = ImGui::RadioButton("Filmic", render_settings->tonemapping == RenderSettings::Filmic) ? RenderSettings::Filmic : render_settings->tonemapping;

				ImGui::TreePop();
			}


			if (ImGui::TreeNode("Bloom"))
			{
				ImGui::Checkbox("Bloom ", &render_settings->bloom);

				ImGui::Text(" Filter and Blur dependance");
				int32 current = render_settings->bloom_dependance;
				if (ImGui::Combo(" ", &current, "Dependent\0Independent\0\0"))
				{
					render_settings->bloom_dependance = (RenderSettings::BloomBlurDependance)current;
				}
				ImGui::SliderFloat("Threshold", &render_settings->bloom_threshold, 0.0, 10.0);
				ImGui::SliderInt("Kernel Size", &render_settings->bloom_kernel_size, 1, 20);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("FXAA"))
			{
				ImGui::Checkbox("FXAA ", &render_settings->fxaa);
				ImGui::SliderFloat("Span max", &render_settings->fxaa_span_max, 0.0f, 40.0f);
				ImGui::SliderFloat("Dir min", &render_settings->fxaa_dir_min, 0.0f, 0.5f);
				ImGui::SliderFloat("Dir reduction", &render_settings->fxaa_dir_reduc, 0.0f, 1.f);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Vignette"))
			{
				ImGui::Checkbox("Vignette ", &render_settings->vigentte);
				ImGui::SliderFloat("Outer raduis", &render_settings->vigentte_outer_raduis, 0.0f, 1.0f);
				ImGui::SliderFloat("Inner raduis", &render_settings->vigentte_inner_raduis, 0.0f, 1.0f);
				ImGui::SliderFloat("Intensity", &render_settings->vigentte_intensity, 0.0f, 1.0f);
				ImGui::TreePop();
			}
		}

		if (ImGui::CollapsingHeader("Performance"))
		{
			String ss = "Time " + std::to_string(delta_time);
			ImGui::PlotLines(ss.c_str(), fps_graph, 100, 0, "", 0, 120, ImVec2(0, 50));
		}

		ImGui::End();
		for (uint32 i = 0; i < 99; i++)
		{
			fps_graph[i] = fps_graph[i + 1];
		}
		fps_graph[99] = delta_time * 1000.f;
	}

	EditorWorldObjectSpawner::EditorWorldObjectSpawner()
	{

	}

	EditorWorldObjectSpawner::EditorWorldObjectSpawner(World *world)
	{
		this->world = world;
	}

	EditorWorldObjectSpawner::~EditorWorldObjectSpawner()
	{

	}

	void EditorWorldObjectSpawner::SpawnCube()
	{
		StaticWorldObject *cube = CreateWorldObjectCube();
		world->RegisterWorldObject(cube);
	}

	void EditorWorldObjectSpawner::SpawnPlane()
	{
		StaticWorldObject *plane = CreateWorldObjectPlane();
		world->RegisterWorldObject(plane);
	}

	void EditorWorldObjectSpawner::UpdateAndDraw()
	{
		Assert(world);
		ImGui::Begin("Spawner");

		if (ImGui::Button("Spawn cube"))
		{
			SpawnCube();
		}
		if (ImGui::Button("Spawn plane"))
		{
			SpawnPlane();
		}

		ImGui::End();
	}

	EditorWorldObjectInspector::EditorWorldObjectInspector()
	{

	}

	EditorWorldObjectInspector::EditorWorldObjectInspector(WorldObject *world_object)
	{
		this->world_object = world_object;
	}

	EditorWorldObjectInspector::~EditorWorldObjectInspector()
	{

	}

	void EditorWorldObjectInspector::SetWorldObject(WorldObject *world_object)
	{
		if (world_object != nullptr)
		{			
			Transform *transform = world_object->GetTransform();
			Quatf rot = transform->rotation;

			if (!Equal(quat_angle, rot))
			{
				euler_angles = QuatToEuler(rot);
			}

			this->world_object = world_object;
		}
		else
		{
			
			this->world_object = nullptr;
		}
	}

	bool EditorWorldObjectInspector::UpdateAndDraw()
	{
		bool value_changed = false;
		ImGui::Begin("Inspector");
		if (world_object)
		{
			if (ImGui::TreeNode("Transform"))
			{
				Transform *transform = world_object->GetTransform();
				ImGui::Text("");
				value_changed = value_changed | ImGui::DragFloat3("Position", transform->position.ptr, 0.1f);
				value_changed = value_changed | ImGui::DragFloat3("Rotation", euler_angles.ptr, 0.5f);
				value_changed = value_changed | ImGui::DragFloat3("Scale", transform->scale.ptr, 0.01f);

				// @NOTE: Apply the rotation
				quat_angle = EulerToQuat(euler_angles);
				transform->rotation = quat_angle;

				GeometricCollider *collider = world_object->GetCollider();
				if (collider) {
					collider->Update(transform);
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Material"))
			{
				Material *mat = world_object->GetMaterial();
				ImGui::ColorEdit3("Diffuse", mat->diffuse.ptr);
				ImGui::DragFloat("Roughness", &mat->roughness, 0.02, 0.0, 1.0);
				ImGui::DragFloat("Metalness", &mat->metalness, 0.02, 0.0, 1.0);
				ImGui::TreePop();
			}
		}

		ImGui::End();

		return value_changed;
	}
}